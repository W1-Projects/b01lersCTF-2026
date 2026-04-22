# Shakespeare’s Revenge

**Category:** Rev

## Description

> William was upset that you solved the last one, so he made the next one a little bit stronger :)  
> `ncat --ssl shakespeares-revenge.opus4-7.b01le.rs 8443`  
> Flag format: `bctf{...}`

## Overview

At first glance, this challenge looks like a language challenge built around a Shakespeare-themed binary. The remote service provides three useful artifacts:

- a Python wrapper
- a `shakespeare` binary
- a `challenge.spl` program

The Python wrapper is not the main target. It only launches the custom `shakespeare` interpreter on the provided Shakespeare source, then prints a random insult after the program exits. fileciteturn2file0L7-L27

That immediately narrows the attack surface to two places:

1. the custom `shakespeare` interpreter
2. the `challenge.spl` script executed by that interpreter

After inspecting the SPL script, the program appears to do nothing more than read numbers, branch on ranges, and perform simple arithmetic. That is only the surface behavior. The real challenge is understanding what the custom interpreter added on top of normal Shakespeare Programming Language semantics.

This makes the challenge primarily a **reverse-engineering challenge**, with a small exploitation step at the end.

## Technical details

### 1. Wrapper analysis

The provided `server.py` is only a launcher. It:

- resolves the current directory
- executes `./shakespeare challenge.spl`
- prints a random insult afterward if `insults.txt` exists fileciteturn2file0L7-L27

So there is nothing meaningful to exploit in Python itself.

### 2. Behavior of `challenge.spl`

The SPL program reads several integer inputs and branches based on one of them. Dynamic testing shows that the third input acts like a selector:

- `0, 1, 2` go to an addition path
- `3` goes to a multiplication path
- `4` goes to a subtraction path
- `>= 5` reaches the final cleanup path

At this stage, the script looks like a tiny arithmetic virtual machine. That appearance is intentional misdirection.

### 3. Custom interpreter instructions

The key reversing step is realizing that this is **not** a stock Shakespeare interpreter.

Two nonstandard instructions are critical:

- `Reference Romeo.`
- `Revere your player Hamlet.`

Reversing the binary shows the following custom behavior.

#### `Reference Romeo.`

This sets Hamlet’s internal reference pointer to Romeo.

#### `Revere your player Hamlet.`

This instruction performs a **raw Linux syscall** using values taken from Hamlet’s stack.

There is also one very important substitution rule:

- if a syscall argument is equal to `0xffffffff`, the interpreter does not pass literal `-1`
- instead, it replaces that value with a pointer to a C-string constructed from the referenced stack
- because the program previously executed `Reference Romeo.`, that referenced stack is Romeo’s stack

So the final cleanup path gives us a constrained syscall primitive where:

- **Hamlet’s stack** controls the syscall number and arguments
- **Romeo’s stack** can be converted into a string pointer

### 4. Why `execve` is the clean solution

An obvious first idea is to try `open`, `read`, and `write` directly. In practice, that path is less reliable because there is no convenient inherited flag file descriptor to dump.

A much cleaner approach is to make the final syscall:

```text
execve("/bin/sh", NULL, NULL)
```

Once the process image is replaced with `/bin/sh`, the network socket becomes our shell I/O channel, and we can send commands such as:

```sh
cat /flag
```

So the real exploit objective becomes:

- build `"/bin/sh\0"` on Romeo’s stack
- build the `execve` syscall frame on Hamlet’s stack
- trigger the cleanup scene

### 5. Turning the arithmetic paths into a builder

Each round of the script lets us feed controlled integers and choose one arithmetic path. By chaining several rounds together, we can synthesize the exact words needed on both stacks.

The final arrangement is:

- Romeo stack contains the bytes for `/bin/sh\0`
- Hamlet stack contains the arguments for `execve`
- one of the syscall arguments is `0xffffffff`, which the interpreter converts into a pointer to Romeo’s constructed string

So when the script reaches the final scene and executes `Revere your player Hamlet.`, the interpreter effectively performs:

```c
syscall(59, "/bin/sh", NULL, NULL);
```

That gives an interactive shell over the challenge connection.

## Proof-of-Concept

Send the following integers, one per line:

```text
0
4294967297
4
104
4294967297
4
493921239087
9223372034707292160
2
60
1
4
472446402665
420906795055
5
```

After sending the numbers, wait briefly, then send:

```sh
cat /flag 2>/dev/null || cat /flag.txt 2>/dev/null || cat flag 2>/dev/null || cat flag.txt 2>/dev/null
exit
```

The short pause matters. If the shell commands are sent too early in the same buffered stream, the interpreter may consume them before the `execve` call happens.

A simple solver workflow is:

1. connect to the TLS service
2. send the 15 numeric inputs
3. wait a short moment
4. send the shell commands
5. read back the flag

This confirms that the final scene is not harmless cleanup logic but the actual exploitation point.

## P/S

This challenge is best categorized as **Rev** rather than Pwn.

The exploitation step is small once the binary is understood. The difficult part is reversing the custom interpreter and identifying that:

- `Revere your player Hamlet.` is actually a syscall primitive
- `Reference Romeo.` determines where string arguments come from
- the arithmetic-looking SPL script is only a frontend for building syscall state

So the intended thought process is:

- ignore the Python wrapper
- reverse the custom interpreter
- understand the hidden semantics of the extra SPL instructions
- use the script as a constrained builder for `execve("/bin/sh", NULL, NULL)`
- read the flag from the spawned shell

