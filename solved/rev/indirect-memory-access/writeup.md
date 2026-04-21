# indirect-memory-access


**Category:** Rev

## Description
> Press Button:
> flag is 'bctf{<above chars>}'
> your emulator sucks :(

## Overview
This challenge validates a 128-slot button-derived bitstream, not a plaintext string. The central weakness is that the ROM's hardware-obfuscated validator (DMA-timed helper calls) collapses to a deterministic 2-input Boolean gate network selected by DMA3SAD constants. Once those gate semantics are mapped, symbolic execution plus SMT directly recovers the unique valid 128-bit stream, then decodes it to the final button sequence.

## Technical details
Exact root cause: the checker is not cryptographically hard; it is an obfuscated but finite Boolean circuit. The obfuscation lives in the validator at 0x08000310 and the IWRAM helper at 0x03000025. Attacker-controlled button input is encoded into a 128-halfword buffer and consumed as circuit leaves.

Evidence of attacker-controlled data flow into the validator:

```asm
0x08001606: lsls   r1, r3, #1
0x08001608: lsls   r4, r0, #0xf
0x0800160a: strh   r4, [r1, r5]      ; write slot to input buffer
...
0x0800162c: cmp    r3, #0x80
0x08001634: bl     #0x8000310         ; call validator after 128 slots
```

Literal pool confirms register roles in that loop:

- r6 -> 0x0300009c (slot counter)
- r5 -> 0x030000a0 (input buffer base)
- alphabet table at 0x08008ba4 = absSRLUDrl

Validator wiring shows where the obfuscated gate execution happens:

```asm
0x08000324: ldr    r3, [pc, #0x318]   ; 0x040000d4 (DMA3SAD)
0x08000328: mov    fp, r3
0x0800032a: str    r2, [r3]           ; select gate by writing DMA3SAD pointer
0x08000330: ldr    r4, [pc, #0x310]   ; 0x030000a0 (input base)
0x08000332: ldr    r5, [pc, #0x314]   ; 0x03000025 (helper entry)
0x08000334: ldrh   r0, [r4, r2]
0x08000336: ldrh   r1, [r4, r3]
0x08000338: bl     #0x80015b2         ; bx r5 -> helper
```

Helper behavior proves DMA-mediated gate evaluation (not ordinary arithmetic):

```asm
0x0800d034: ldr    r2, [pc, #0x2c]    ; 0x040000de (DMA3CNT_H)
0x0800d03e: ldr    r3, [pc, #0x28]    ; 0x040000ba (DMA0CNT_H)
0x0800d046: ldr    r3, [pc, #0x24]    ; 0x040000c6 (DMA1CNT_H)
0x0800d050: ldr    r0, [pc, #0x20]    ; 0x0200000b (unaligned read source)
0x0800d052: ldr    r0, [r0]
0x0800d054: lsls   r0, r0, #0x10
0x0800d056: lsrs   r0, r0, #0x10      ; return low 16 bits
```

Primitive gained: full control of 128 Boolean leaves x0..x127 (encoded as 0x0000/0x8000), while the circuit topology is fixed by validator instruction order and DMA3SAD literals. This is sufficient because the final check is only the truth value of the circuit root:

```asm
0x08001590: bl     #0x80015b0
0x08001594: subs   r3, r0, #1
0x08001596: sbcs   r0, r3             ; canonicalize nonzero -> true
```

solve.py implements the exact abstraction used to exploit this:

```python
GATES = {
    0x08008B7C: lambda a, b: Or(Not(a), b),
    0x08008B7E: lambda a, b: Or(a, b),
    0x08008B84: lambda a, b: Not(And(a, b)),
    0x08008B86: lambda a, b: And(a, b),
    0x08008B88: lambda a, b: Xor(a, b),
    0x08008B8E: lambda a, b: Not(Or(a, b)),
    0x08008B94: lambda a, b: Or(a, Not(b)),
}
```

```python
elif mnemonic == "bl":
    target = int(insn.op_str.strip("#"), 16)
    if target not in (0x080015AE, 0x080015B0, 0x080015B2):
        raise RuntimeError(...)
    gate_ptr = self.read32(DMA3SAD)
    gate = GATES[gate_ptr]
    left = boolify(self.get_reg("r0"))
    right = boolify(self.get_reg("r1"))
    self.set_reg("r0", gate(left, right))
```

Why exploit works: replacing each helper call with its gate semantics transforms the entire validator into one SMT formula over 128 booleans; SAT yields the exact valid input accepted by the ROM.

## Proof-of-Concept
- Step 1: Reconstruct gate semantics selected by DMA3SAD.
Purpose: turn hardware side effects into deterministic logical operators.

```python
GATES = {
    0x08008B86: lambda a, b: And(a, b),
    0x08008B88: lambda a, b: Xor(a, b),
    0x08008B8E: lambda a, b: Not(Or(a, b)),
    # ... remaining gate selectors
}
```

Artifact: a total mapping from DMA3SAD constants to Boolean gates, enabling pure symbolic lifting.

- Step 2: Model attacker-controlled input exactly at checker memory layout.
Purpose: expose controllable leaves to the solver with the same addressing used by ROM code.

```python
self.inputs = [Bool(f"x{i}") for i in range(128)]
for i, bit in enumerate(self.inputs):
    self.mem16[INPUT_BASE + 2 * i] = bit
```

Artifact: 128 symbolic leaves x0..x127 corresponding to slots at 0x030000a0.

- Step 3: Lift validator control flow and replace helper calls with gate applications.
Purpose: build the full circuit expression without emulating cycle-accurate GBA DMA timing.

```python
validator = SymbolicValidator()
expr = validator.run()
```

Artifact: one root Boolean expression expr representing the acceptance condition.

- Step 4: Solve and verify uniqueness.
Purpose: obtain the exact accepted 128-bit stream and prove it is unique.

```python
solver = Solver()
solver.add(expr)
model = solver.model()
bits = "".join("1" if is_true(model.evaluate(bit, model_completion=True)) else "0" for bit in validator.inputs)
solver.add(Or([var != is_true(model.evaluate(var, model_completion=True)) for var in validator.inputs]))
unique = solver.check() != sat
```

Artifact: unique_solution = True and recovered bitstream.

- Step 5: Decode unary stream to button tokens and flag.
Purpose: convert SAT output to the ROM's expected button alphabet.

```python
def decode_buttons(bits: str) -> str:
    alphabet = "absSRLUDrl"
    out, zeros = [], 0
    for bit in bits:
        if bit == "0":
            zeros += 1
            continue
        out.append(alphabet[zeros])
        zeros = 0
    return "".join(out)
```

Artifact produced by solve.py:

```text
unique_solution = True
bitstream       = 10001011101101101111010110011100010100111110010000011000010000101110000000111110000000101000011101100110000111010010100011011001
buttons         = aSbaabababaaabbasaaSbsaaaasLaRRbaaDaaaaDbRaabasaRaabsbSabas
flag            = bctf{aSbaabababaaabbasaaSbsaaaasLaRRbaaDaaaaDbRaabasaRaabsbSabas}
```

## P/S
The anti-emulator check is a reliability barrier, not a cryptographic one. Once helper semantics are lifted to logic gates, the challenge reduces to deterministic constraint solving, which avoids fragile cycle-accurate emulation entirely.
