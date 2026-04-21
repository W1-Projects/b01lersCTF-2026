from __future__ import annotations

from pathlib import Path

from capstone import CS_ARCH_ARM, CS_MODE_THUMB, Cs
from capstone.arm import (
    ARM_OP_IMM,
    ARM_OP_MEM,
    ARM_OP_REG,
    ARM_REG_FP,
    ARM_REG_IP,
    ARM_REG_LR,
    ARM_REG_PC,
    ARM_REG_R0,
    ARM_REG_R1,
    ARM_REG_R2,
    ARM_REG_R3,
    ARM_REG_R4,
    ARM_REG_R5,
    ARM_REG_R6,
    ARM_REG_R7,
    ARM_REG_R8,
    ARM_REG_SB,
    ARM_REG_SL,
    ARM_REG_SP,
)
from z3 import And, Bool, BoolRef, BoolVal, Not, Or, Solver, Xor, is_true, sat


ROM_BASE = 0x08000000
VALIDATOR = 0x08000310
INPUT_BASE = 0x030000A0
DMA3SAD = 0x040000D4


# The helper behaves like a 2-input gate selected by DMA3SAD.
# These truth tables fall straight out of the DMA3 -> DMA1 -> DMA0(HBlank)
# interaction in IWRAM helper 0x03000025.
GATES = {
    0x08008B7C: lambda a, b: Or(Not(a), b),  # x -> y
    0x08008B7E: lambda a, b: Or(a, b),
    0x08008B84: lambda a, b: Not(And(a, b)),
    0x08008B86: lambda a, b: And(a, b),
    0x08008B88: lambda a, b: Xor(a, b),
    0x08008B8E: lambda a, b: Not(Or(a, b)),
    0x08008B94: lambda a, b: Or(a, Not(b)),  # y -> x
}

REG_NAMES = {
    ARM_REG_R0: "r0",
    ARM_REG_R1: "r1",
    ARM_REG_R2: "r2",
    ARM_REG_R3: "r3",
    ARM_REG_R4: "r4",
    ARM_REG_R5: "r5",
    ARM_REG_R6: "r6",
    ARM_REG_R7: "r7",
    ARM_REG_R8: "r8",
    ARM_REG_SB: "sb",
    ARM_REG_SL: "sl",
    ARM_REG_FP: "fp",
    ARM_REG_IP: "r12",
    ARM_REG_SP: "sp",
    ARM_REG_LR: "lr",
    ARM_REG_PC: "pc",
}


def reg_name(op) -> str:
    return REG_NAMES[op.reg]


def reg_list(op_str: str) -> list[str]:
    return [part.strip() for part in op_str.strip("{}").split(",") if part.strip()]


def is_int(value) -> bool:
    return isinstance(value, int)


def boolify(value) -> BoolRef:
    if isinstance(value, BoolRef):
        return value
    if value == 0:
        return BoolVal(False)
    if value in (0x8000, 0xFFFFFFFF):
        return BoolVal(True)
    raise ValueError(f"expected boolean-like value, got {value!r}")


class SymbolicValidator:
    def __init__(self, rom_path: str = "dist/chal.gba") -> None:
        self.rom = Path(rom_path).read_bytes()
        self.md = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
        self.md.detail = True
        self.cache = {}

        self.regs = {f"r{i}": None for i in range(13)}
        self.regs.update({"sp": 0x100000, "lr": None, "pc": VALIDATOR, "sb": None, "sl": None, "fp": None, "r8": None})
        self.mem32 = {}
        self.mem16 = {}
        self.inputs = [Bool(f"x{i}") for i in range(128)]

        for i, bit in enumerate(self.inputs):
            self.mem16[INPUT_BASE + 2 * i] = bit

    def disasm_one(self, address: int):
        if address not in self.cache:
            offset = address - ROM_BASE
            self.cache[address] = next(self.md.disasm(self.rom[offset : offset + 4], address), None)
        return self.cache[address]

    def get_reg(self, name: str):
        return self.regs.get(name)

    def set_reg(self, name: str, value) -> None:
        self.regs[name] = value

    def rom32(self, address: int) -> int:
        offset = address - ROM_BASE
        return int.from_bytes(self.rom[offset : offset + 4], "little")

    def read32(self, address: int):
        return self.mem32.get(address)

    def write32(self, address: int, value) -> None:
        self.mem32[address] = value

    def read16(self, address: int):
        return self.mem16.get(address)

    def write16(self, address: int, value) -> None:
        self.mem16[address] = value

    def eval_ea(self, operand, insn) -> int:
        mem = operand.mem
        if mem.base == ARM_REG_PC:
            base_value = (insn.address + 4) & ~3
        else:
            base_value = self.get_reg(REG_NAMES[mem.base]) if mem.base else 0
        index_value = self.get_reg(REG_NAMES[mem.index]) if mem.index else 0
        if not (is_int(base_value) and is_int(index_value)):
            raise ValueError(f"non-constant effective address at {insn.address:#x}: {insn.mnemonic} {insn.op_str}")
        return base_value + index_value + mem.disp

    def run(self) -> BoolRef:
        address = VALIDATOR
        steps = 0

        while True:
            steps += 1
            if steps > 5000:
                raise RuntimeError("validator walk exceeded expected length")

            insn = self.disasm_one(address)
            if insn is None:
                raise RuntimeError(f"failed to disassemble at {address:#x}")

            mnemonic = insn.mnemonic

            if mnemonic == "push":
                for reg in reg_list(insn.op_str):
                    self.regs["sp"] -= 4
                    self.write32(self.regs["sp"], self.get_reg(reg))
            elif mnemonic == "pop":
                for reg in reg_list(insn.op_str):
                    self.set_reg(reg, self.read32(self.regs["sp"]))
                    self.regs["sp"] += 4
            elif mnemonic in ("mov", "movs"):
                dst = reg_name(insn.operands[0])
                src = insn.operands[1]
                value = self.get_reg(reg_name(src)) if src.type == ARM_OP_REG else src.imm
                self.set_reg(dst, value)
            elif mnemonic in ("add", "adds"):
                dst = reg_name(insn.operands[0])
                if len(insn.operands) == 2:
                    left = self.get_reg(dst)
                    src = insn.operands[1]
                    right = self.get_reg(reg_name(src)) if src.type == ARM_OP_REG else src.imm
                else:
                    src1, src2 = insn.operands[1], insn.operands[2]
                    left = self.get_reg(reg_name(src1)) if src1.type == ARM_OP_REG else src1.imm
                    right = self.get_reg(reg_name(src2)) if src2.type == ARM_OP_REG else src2.imm
                self.set_reg(dst, left + right if is_int(left) and is_int(right) else None)
            elif mnemonic == "subs":
                dst = reg_name(insn.operands[0])
                if len(insn.operands) == 2:
                    left = self.get_reg(dst)
                    src = insn.operands[1]
                    right = self.get_reg(reg_name(src)) if src.type == ARM_OP_REG else src.imm
                else:
                    src1, src2 = insn.operands[1], insn.operands[2]
                    left = self.get_reg(reg_name(src1)) if src1.type == ARM_OP_REG else src1.imm
                    right = self.get_reg(reg_name(src2)) if src2.type == ARM_OP_REG else src2.imm
                self.set_reg(dst, left - right if is_int(left) and is_int(right) else None)
            elif mnemonic == "lsls":
                dst = reg_name(insn.operands[0])
                src = insn.operands[1]
                shift = insn.operands[2].imm
                value = self.get_reg(reg_name(src)) if src.type == ARM_OP_REG else src.imm
                self.set_reg(dst, (value << shift) & 0xFFFFFFFF if is_int(value) else None)
            elif mnemonic == "ldr":
                dst = reg_name(insn.operands[0])
                address_value = self.eval_ea(insn.operands[1], insn)
                if ROM_BASE <= address_value < ROM_BASE + len(self.rom):
                    self.set_reg(dst, self.rom32(address_value))
                else:
                    self.set_reg(dst, self.read32(address_value))
            elif mnemonic == "ldrh":
                dst = reg_name(insn.operands[0])
                address_value = self.eval_ea(insn.operands[1], insn)
                value = self.read16(address_value)
                if value is None:
                    raise RuntimeError(f"read from unknown halfword {address_value:#x} at {insn.address:#x}")
                self.set_reg(dst, value)
            elif mnemonic == "str":
                address_value = self.eval_ea(insn.operands[1], insn)
                self.write32(address_value, self.get_reg(reg_name(insn.operands[0])))
            elif mnemonic == "sbcs":
                # The validator ends with:
                #   subs r3, r0, #1
                #   sbcs r0, r3
                # This just canonicalizes nonzero -> true.
                current = self.get_reg(reg_name(insn.operands[0]))
                if not isinstance(current, BoolRef):
                    self.set_reg(reg_name(insn.operands[0]), None)
            elif mnemonic == "bl":
                target = int(insn.op_str.strip("#"), 16)
                if target not in (0x080015AE, 0x080015B0, 0x080015B2):
                    raise RuntimeError(f"unexpected branch-and-link target {target:#x}")
                gate_ptr = self.read32(DMA3SAD)
                gate = GATES[gate_ptr]
                left = boolify(self.get_reg("r0"))
                right = boolify(self.get_reg("r1"))
                self.set_reg("r0", gate(left, right))
            elif mnemonic == "b":
                address = int(insn.op_str.strip("#"), 16)
                continue
            elif mnemonic == "bx":
                return self.get_reg("r0")
            else:
                raise RuntimeError(f"unhandled instruction {insn.address:#x}: {mnemonic} {insn.op_str}")

            address += insn.size


def decode_buttons(bits: str) -> str:
    alphabet = "absSRLUDrl"
    out = []
    zeros = 0
    for bit in bits:
        if bit == "0":
            zeros += 1
            continue
        out.append(alphabet[zeros])
        zeros = 0
    if zeros:
        raise ValueError("bitstream does not end cleanly on a token boundary")
    return "".join(out)


def main() -> None:
    validator = SymbolicValidator()
    expr = validator.run()

    solver = Solver()
    solver.add(expr)
    if solver.check() != sat:
        raise RuntimeError("validator formula is unsat")

    model = solver.model()
    bits = "".join("1" if is_true(model.evaluate(bit, model_completion=True)) else "0" for bit in validator.inputs)

    # Prove uniqueness of the recovered 128-bit stream.
    solver.add(Or([var != is_true(model.evaluate(var, model_completion=True)) for var in validator.inputs]))
    unique = solver.check() != sat

    buttons = decode_buttons(bits)
    print(f"unique_solution = {unique}")
    print(f"bitstream       = {bits}")
    print(f"buttons         = {buttons}")
    print(f"flag            = bctf{{{buttons}}}")


if __name__ == "__main__":
    main()
