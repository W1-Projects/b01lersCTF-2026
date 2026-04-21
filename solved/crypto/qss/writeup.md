# qss

**Category:** Crypto

## Description
> The original organizer description was not included in the provided artifacts, and the public challenge page is login-gated.

## Overview
This challenge is a broken quantum secret-sharing simulator, not a hard decoding problem. The core flaw is that the server accepts an attacker-chosen `8x8` unitary and applies it to the entire three-qubit register `(a, b, c)`, including Alice's qubit, before Alice is measured. The solve uses `U = SWAP(a, c) * H(a) * CNOT(a -> b)` with ancilla `|0>` to decode the Bell-encoded logical qubit onto Bob/Charlie-controlled qubits, recover every kept `info`-round secret exactly, and then retry until the statistically noisy `test` score passes.
```
from __future__ import annotations

import json
import os
import random
import sys
from typing import Dict, List, Tuple

import numpy as np

try:
    from qiskit import QuantumCircuit, transpile
    from qiskit.providers.basic_provider import BasicSimulator
    from qiskit.quantum_info import Operator
except Exception:
    QuantumCircuit = None
    transpile = None
    BasicSimulator = None
    Operator = None

ROUNDS = 96
TEST_RATIO = 0.40
MAX_QBER = 0.32
MIN_QBER_CHECKED = 4
MIN_QBER_CHECKED_PER_BASIS = 2
MIN_INFO_ROUNDS = 16
MIN_MUTUAL_INFO_BITS = 0.5
FRONTIER_GAP_TOL_BITS = 0.25
MIN_RAW_INFO_ACCURACY = 0.60

FLAG = "REDACTED"


def binary_entropy(p: float) -> float:
    p = min(max(p, 0.0), 1.0)
    if p <= 1e-15 or p >= 1.0 - 1e-15:
        return 0.0
    return -(p * np.log2(p) + (1.0 - p) * np.log2(1.0 - p))


def mutual_information_from_error(q: float) -> float:
    return 1.0 - binary_entropy(q)


def theory_max_information_from_qber(d: float) -> Tuple[float, float, float]:
    d = min(max(d, 0.0), 0.5)
    a = min(max(4.0 * d - 1.0, -1.0), 1.0)
    q_min = 0.5 - 0.5 * float(np.sqrt(max(0.0, 1.0 - a * a)))
    i_max = mutual_information_from_error(q_min)
    return a, q_min, i_max

def parse_complex(value):
    if isinstance(value, (int, float)):
        return complex(value, 0.0)
    if isinstance(value, str):
        text = value.strip().replace("I", "j").replace("i", "j")
        return complex(text)
    raise ValueError(f"Unsupported complex value: {value!r}")


def parse_ancilla(line: str) -> np.ndarray:
    raw = json.loads(line)
    if not isinstance(raw, list) or len(raw) != 2:
        raise ValueError("Ancilla must be a JSON list of 2 entries")

    vec = np.array([parse_complex(raw[0]), parse_complex(raw[1])], dtype=np.complex128)
    norm = float(np.vdot(vec, vec).real)
    if abs(norm - 1.0) > 1e-6:
        raise ValueError(f"Ancilla is not normalized (norm={norm:.9f})")
    return vec


def parse_unitary(line: str) -> np.ndarray:
    raw = json.loads(line)
    if not isinstance(raw, list) or len(raw) != 8:
        raise ValueError("Unitary must be a JSON 8x8 list")

    rows: List[List[complex]] = []
    for row in raw:
        if not isinstance(row, list) or len(row) != 8:
            raise ValueError("Unitary must be a JSON 8x8 list")
        rows.append([parse_complex(v) for v in row])

    return np.array(rows, dtype=np.complex128)


def permute_abc_to_qiskit_matrix() -> np.ndarray:
    p = np.zeros((8, 8), dtype=np.complex128)
    for a in (0, 1):
        for b in (0, 1):
            for c in (0, 1):
                user_idx = (a << 2) | (b << 1) | c
                qiskit_idx = (c << 2) | (b << 1) | a
                p[qiskit_idx, user_idx] = 1.0
    return p


def build_bell_states_user() -> Dict[Tuple[int, int], np.ndarray]:
    phi_minus = np.array([1, 0, 0, -1], dtype=np.complex128) / np.sqrt(2.0)
    psi_plus = np.array([0, 1, 1, 0], dtype=np.complex128) / np.sqrt(2.0)
    psi_upper_plus = (phi_minus + psi_plus) / np.sqrt(2.0)
    phi_upper_minus = (phi_minus - psi_plus) / np.sqrt(2.0)

    states: Dict[Tuple[int, int], np.ndarray] = {}
    states[(0, 0)] = phi_minus
    states[(0, 1)] = psi_plus
    states[(1, 0)] = phi_upper_minus
    states[(1, 1)] = psi_upper_plus
    return states


def execute_round_once(
    backend,
    bell_user: np.ndarray,
    ancilla_user: np.ndarray,
    perm: np.ndarray,
    unitary_op,
    alice_basis: str,
    plan: List[Tuple[int, str]],
) -> Dict[int, int]:
    """Build and execute one protocol circuit shot, returning measured bits by qubit index."""
    state_user = np.kron(bell_user, ancilla_user)
    state_qiskit = perm @ state_user

    qc = QuantumCircuit(3, 3)
    qc.initialize(state_qiskit, [0, 1, 2])
    qc.append(unitary_op, [0, 1, 2])

    if alice_basis == "x":
        qc.h(0)
    qc.measure(0, 0)

    for qubit, basis in plan:
        if basis == "x":
            qc.h(qubit)
        qc.measure(qubit, qubit)

    compiled = transpile(qc, backend)
    result = backend.run(compiled, shots=1, memory=True).result()
    memory = result.get_memory()[0]

    return {
        0: int(memory[-1]),
        1: int(memory[-2]),
        2: int(memory[-3]),
    }


def parse_measurement_plan(line: str) -> List[Tuple[int, str]]:
    raw = json.loads(line)
    if not isinstance(raw, list):
        raise ValueError("Measurement plan must be a JSON list")

    if len(raw) == 0 or len(raw) > 2:
        raise ValueError("Measurement plan must contain 1 or 2 measurements")

    qubit_name_to_idx = {"b": 1, "c": 2}
    seen = set()
    plan: List[Tuple[int, str]] = []

    for item in raw:
        if (
            not isinstance(item, list)
            or len(item) != 2
            or not isinstance(item[0], str)
            or not isinstance(item[1], str)
        ):
            raise ValueError("Each measurement must be [\"b|c\", \"z|x\"]")

        qubit_name = item[0].strip().lower()
        basis = item[1].strip().lower()
        if qubit_name not in qubit_name_to_idx:
            raise ValueError("Qubit must be 'b' or 'c'")
        if basis not in ("z", "x"):
            raise ValueError("Basis must be 'z' or 'x'")

        q = qubit_name_to_idx[qubit_name]
        if q in seen:
            raise ValueError("Each qubit can be measured at most once per round")
        seen.add(q)
        plan.append((q, basis))

    return plan


def parse_pre_public_announcement(line: str) -> Tuple[str, int]:
    raw = json.loads(line)
    if not isinstance(raw, dict):
        raise ValueError("Announcement must be a JSON object")

    basis = raw.get("basis")
    outcome = raw.get("outcome")

    if not isinstance(basis, str):
        raise ValueError("Announcement field 'basis' must be a string")
    basis = basis.strip().lower()
    if basis not in ("z", "x"):
        raise ValueError("Announcement basis must be 'z' or 'x'")

    if not isinstance(outcome, int) or outcome not in (0, 1):
        raise ValueError("Announcement field 'outcome' must be 0 or 1")

    return basis, outcome


def expected_bob_bit_for_check(set_bit: int, secret: int, alice_bit: int) -> int:
    if secret == 0:
        return alice_bit
    return 1 - alice_bit


def read_line_or_die(prompt: str) -> str:
    print(prompt, flush=True)
    line = sys.stdin.readline()
    if line == "":
        raise EOFError("Unexpected EOF")
    return line.strip()


def main() -> None:
    if Operator is None or QuantumCircuit is None or BasicSimulator is None:
        print("Missing dependency: qiskit")
        print("Install with: pip install qiskit")
        return

    rng = random.Random()
    backend = BasicSimulator()
    bell_states_user = build_bell_states_user()
    perm = permute_abc_to_qiskit_matrix()

    try:
        ancilla_line = read_line_or_die("ancilla_statevector_json>")
        ancilla_user = parse_ancilla(ancilla_line)

        unitary_line = read_line_or_die("unitary_8x8_json>")
        unitary_user = parse_unitary(unitary_line)
        unitary_qiskit = perm @ unitary_user @ perm.T
        unitary_op = Operator(unitary_qiskit)
        if not unitary_op.is_unitary():
            raise ValueError("Submitted matrix is not unitary (Qiskit validation failed)")
    except Exception as exc:
        print(f"Input error: {exc}")
        return

    info_correct = 0
    info_rounds = 0
    info_total_rounds = 0
    qber_checked = 0
    qber_errors = 0
    qber_checked_by_basis = {"z": 0, "x": 0}
    qber_errors_by_basis = {"z": 0, "x": 0}
    announce_counts = {"z": 0, "x": 0}

    for round_idx in range(ROUNDS):
        secret = rng.randint(0, 1)
        set_bit = rng.randint(0, 1)
        alice_basis = rng.choice(["z", "x"])
        is_test_round = rng.random() < TEST_RATIO

        print(f"--- round {round_idx + 1}/{ROUNDS} ---")
        print(f"round_type: {'test' if is_test_round else 'info'}")

        # Paper-style constraint: Bob must announce test outcomes before
        # Trent/Alice public disclosure.
        try:
            announce_line = read_line_or_die("pre_public_announce_json>")
            announce_basis, announce_outcome = parse_pre_public_announcement(announce_line)
            announce_counts[announce_basis] += 1
        except Exception as exc:
            print(f"Round input error: {exc}")
            return

        if set_bit == 0:
            print("public_set: S0 = {|psi+>, |phi->}")
        else:
            print("public_set: S1 = {|Psi+>, |Phi->}")
        print(f"public_alice_basis: {alice_basis}")
        required_basis = "z" if set_bit == 0 else "x"

        try:
            bell_user = bell_states_user[(set_bit, secret)]

            if is_test_round:
                # Test rounds only need Alice's measured bit for consistency check.
                measured_bits = execute_round_once(
                    backend=backend,
                    bell_user=bell_user,
                    ancilla_user=ancilla_user,
                    perm=perm,
                    unitary_op=unitary_op,
                    alice_basis=alice_basis,
                    plan=[],
                )
                alice_bit = measured_bits[0]

                # Check-valid subset mirrors paper's basis-consistency filtering.
                # Check rounds are only valid when Trent's set, Alice's basis,
                # and Bob's declared basis are basis-consistent.
                check_valid = (
                    alice_basis == required_basis and announce_basis == required_basis
                )
                if check_valid:
                    qber_checked += 1
                    qber_checked_by_basis[announce_basis] += 1
                    expected = expected_bob_bit_for_check(set_bit, secret, alice_bit)
                    if announce_outcome != expected:
                        qber_errors += 1
                        qber_errors_by_basis[announce_basis] += 1
                print(f"test_check_valid: {'yes' if check_valid else 'no'}")
            else:
                info_total_rounds += 1
                # Information rounds only contribute key material when the
                # announced Bob basis matches Alice's basis.
                info_keep = announce_basis == alice_basis
                print(f"info_keep: {'yes' if info_keep else 'no'}")
                if not info_keep:
                    continue

                info_rounds += 1
                plan_line = read_line_or_die("measurement_plan_json>")
                plan = parse_measurement_plan(plan_line)

                measured_bits = execute_round_once(
                    backend=backend,
                    bell_user=bell_user,
                    ancilla_user=ancilla_user,
                    perm=perm,
                    unitary_op=unitary_op,
                    alice_basis=alice_basis,
                    plan=plan,
                )

                outcomes: List[str] = []
                for qubit, basis in plan:
                    bit = measured_bits[qubit]
                    qubit_name = "b" if qubit == 1 else "c"
                    outcomes.append(f"{qubit_name}:{basis}={bit}")
                print("measurement_outcomes:", " ".join(outcomes))

                guess_line = read_line_or_die("secret_guess_bit>")
                guess = int(guess_line)
                if guess not in (0, 1):
                    raise ValueError("Secret guess must be 0 or 1")

                if guess == secret:
                    info_correct += 1
        except Exception as exc:
            print(f"Round input error: {exc}")
            return

    kept_info_accuracy = (info_correct / info_rounds) if info_rounds else 0.5
    info_error = 1.0 - kept_info_accuracy
    mutual_info_empirical = mutual_information_from_error(info_error)
    dropped_info_rounds = info_total_rounds - info_rounds
    # Calibrated raw metric: dropped rounds contribute neutral 0.5 success
    # because no key bit is extracted from those basis-inconsistent rounds.
    raw_info_accuracy = (
        (info_correct + 0.5 * dropped_info_rounds) / info_total_rounds
        if info_total_rounds
        else 0.5
    )
    raw_info_error = 1.0 - raw_info_accuracy
    raw_mutual_info_empirical = mutual_information_from_error(raw_info_error)
    qber = (qber_errors / qber_checked) if qber_checked else 1.0
    a_est, qmin_theory, mutual_info_theory_max = theory_max_information_from_qber(qber)
    frontier_gap_bits = mutual_info_theory_max - mutual_info_empirical
    frontier_abs_dev_bits = abs(frontier_gap_bits)

    announce_total = announce_counts["z"] + announce_counts["x"]
    announce_balance = (
        abs(announce_counts["z"] - announce_counts["x"]) / announce_total
        if announce_total
        else 1.0
    )

    qber_z = (
        qber_errors_by_basis["z"] / qber_checked_by_basis["z"]
        if qber_checked_by_basis["z"]
        else 0.0
    )
    qber_x = (
        qber_errors_by_basis["x"] / qber_checked_by_basis["x"]
        if qber_checked_by_basis["x"]
        else 0.0
    )

    print()
    print("=== score ===")
    print(f"rounds={ROUNDS}")
    print(f"info_rounds={info_rounds}")
    print(f"info_total_rounds={info_total_rounds}")
    print(f"correct={info_correct}")
    print(f"kept_info_accuracy={kept_info_accuracy:.4f}")
    print(f"info_error={info_error:.4f}")
    print(f"raw_info_accuracy={raw_info_accuracy:.4f}")
    print(
        f"qber_checked_by_basis=z:{qber_checked_by_basis['z']} x:{qber_checked_by_basis['x']}"
    )
    print(f"qber_by_basis=z:{qber_z:.4f} x:{qber_x:.4f}")
    print(f"qber={qber:.4f}")
    print(f"max_qber={MAX_QBER:.4f}")
    print(f"min_info_rounds={MIN_INFO_ROUNDS}")
    print(f"min_mutual_info_bits={MIN_MUTUAL_INFO_BITS:.4f}")
    print(f"min_qber_checked={MIN_QBER_CHECKED}")
    print(f"min_qber_checked_per_basis={MIN_QBER_CHECKED_PER_BASIS}")
    print(f"max_frontier_abs_dev_bits={FRONTIER_GAP_TOL_BITS:.4f}")

    if qber_checked < MIN_QBER_CHECKED:
        print(
            f"No flag yet: insufficient valid test checks ({qber_checked} < {MIN_QBER_CHECKED})."
        )
    elif (
        qber_checked_by_basis["z"] < MIN_QBER_CHECKED_PER_BASIS
        or qber_checked_by_basis["x"] < MIN_QBER_CHECKED_PER_BASIS
    ):
        print(
            "No flag yet: insufficient basis-balanced test checks "
            f"(z={qber_checked_by_basis['z']}, x={qber_checked_by_basis['x']}, "
            f"need >= {MIN_QBER_CHECKED_PER_BASIS} each)."
        )
    elif raw_info_accuracy < MIN_RAW_INFO_ACCURACY:
        print("No flag yet: raw info accuracy too low")
    elif info_rounds < MIN_INFO_ROUNDS:
        print(
            f"No flag yet: insufficient kept information rounds ({info_rounds} < {MIN_INFO_ROUNDS})."
        )
    elif qber > MAX_QBER:
        print("ABORT: QBER too high. Attack detected during test rounds.")
    elif mutual_info_empirical < MIN_MUTUAL_INFO_BITS:
        print(
            "No flag yet: mutual information too low for a paper-optimal attack "
            f"({mutual_info_empirical:.4f} < {MIN_MUTUAL_INFO_BITS:.4f})."
        )
    elif frontier_abs_dev_bits > FRONTIER_GAP_TOL_BITS:
        print(
            "No flag yet: observed point is too far from paper tradeoff frontier "
            f"(|I_max(D)-I_emp|={frontier_abs_dev_bits:.4f} > {FRONTIER_GAP_TOL_BITS:.4f})."
        )
    elif announce_balance > 0.50:
        print(
            "No flag yet: announced pre-public bases are too imbalanced; "
            f"use both z and x (balance={announce_balance:.4f})."
        )
    elif mutual_info_empirical <= raw_mutual_info_empirical:
        print(
            "No flag yet: delayed-information advantage not demonstrated on kept rounds."
        )
    else:
        print(FLAG)


if __name__ == "__main__":
    main()

```
## Technical details
The trust-boundary failure is in the setup path. The server reads an attacker-controlled ancilla and an attacker-controlled `8x8` unitary, converts the matrix into Qiskit's qubit order, and applies it to all three qubits:

```python
ancilla_user = parse_ancilla(ancilla_line)
unitary_user = parse_unitary(unitary_line)
unitary_qiskit = perm @ unitary_user @ perm.T
unitary_op = Operator(unitary_qiskit)
...
qc.initialize(state_qiskit, [0, 1, 2])
qc.append(unitary_op, [0, 1, 2])
```

That is the root cause. The intended role is an eavesdropper with an ancilla, but `qc.append(unitary_op, [0, 1, 2])` grants a fully arbitrary joint operation on Alice, Bob, and the ancilla. The player is therefore not limited to extracting partial side information; they can rewrite the protocol state before Alice's measurement.

The secret is encoded in four two-qubit states built by `build_bell_states_user()`:

```python
states[(0, 0)] = phi_minus
states[(0, 1)] = psi_plus
states[(1, 0)] = phi_upper_minus
states[(1, 1)] = psi_upper_plus
```

with

```python
phi_minus = (|00> - |11>) / sqrt(2)
psi_plus  = (|01> + |10>) / sqrt(2)
phi_upper_minus = (phi_minus - psi_plus) / sqrt(2)
psi_upper_plus  = (phi_minus + psi_plus) / sqrt(2)
```

All four states live in the same two-dimensional subspace `span{|phi->, |psi+>}`. The submitted unitary from `solve.py` is documented as:

```python
# U = SWAP(a, c) * H(a) * CNOT(a -> b)
ANCILLA = [1.0, 0.0]
```

Read right-to-left, this first applies `CNOT(a -> b)` and `H(a)`, which is Bell-basis decoding on `(a, b)`, then swaps Alice's decoded qubit into the ancilla position. For the two `S0` basis states, the transformation is:

```text
U(|phi->_ab |0>_c) = |001>_abc
U(|psi+>_ab |0>_c) = |010>_abc
```

So after the rewrite, `b` alone stores the secret in the computational basis:

- `S0, secret=0` gives `b=0`
- `S0, secret=1` gives `b=1`

For the two `S1` states, linearity gives:

```text
U(|Phi->_ab |0>_c) = (|001> - |010>) / sqrt(2)
U(|Psi+>_ab |0>_c) = (|001> + |010>) / sqrt(2)
```

Equivalently, Alice is factored out as `|0>_a`, and `(b, c)` become one of two Bell states:

```text
|0>_a tensor (|01> - |10>) / sqrt(2)
|0>_a tensor (|01> + |10>) / sqrt(2)
```

Measuring both qubits in the `X` basis distinguishes them by correlation:

- equal `X` outcomes correspond to `(|01> + |10>) / sqrt(2)` and therefore `secret = 1`
- different `X` outcomes correspond to `(|01> - |10>) / sqrt(2)` and therefore `secret = 0`

The second design mistake is that `info` rounds reveal enough public data before the attacker commits to the Bob/Charlie measurement plan:

```python
print("public_set: ...")
print(f"public_alice_basis: {alice_basis}")
...
plan_line = read_line_or_die("measurement_plan_json>")
plan = parse_measurement_plan(plan_line)
measured_bits = execute_round_once(..., plan=plan)
```

So the attacker gets an adaptive discrimination primitive:

- if `public_set` is `S0`, measure `b` in `Z`
- if `public_set` is `S1`, measure `b` and `c` in `X`

This is already enough to solve the real objective, because `info` rounds score only whether the guessed `secret` is correct:

```python
if guess == secret:
    info_correct += 1
```

The script also handles the scoring constraints correctly. Kept `info` rounds require only `announce_basis == alice_basis`:

```python
info_keep = announce_basis == alice_basis
```

so alternating announced bases yields roughly half of all `info` rounds as usable samples while keeping the announcement distribution balanced. Since dropped `info` rounds contribute a neutral `0.5` in

```python
raw_info_accuracy = (info_correct + 0.5 * dropped_info_rounds) / info_total_rounds
```

perfect guesses on kept rounds are enough to clear both `MIN_INFO_ROUNDS` and `MIN_RAW_INFO_ACCURACY`.

`test` rounds are the only non-deterministic part of the exploit. The server requires the attacker to announce a basis and outcome before `public_set`, `secret`, and Alice's actual measurement result are known:

```python
announce_line = read_line_or_die("pre_public_announce_json>")
...
check_valid = alice_basis == required_basis and announce_basis == required_basis
expected = expected_bob_bit_for_check(set_bit, secret, alice_bit)
```

For the chosen `U`, Alice is always in `|0>`. That means checked `test` rounds still depend on hidden data: on `Z` checks the required answer depends on the unknown `secret`, and on `X` checks it depends on both `secret` and Alice's random `X`-basis outcome. The solve therefore does not try to beat the test oracle; it fixes `outcome = 0` and accepts an approximately random error rate on checked tests.

This still works because only a small subset of test rounds are counted (`check_valid`), and the final acceptance window is not simply `qber <= 0.32`. The code also requires the empirical point to stay near the paper frontier:

```python
a = 4.0 * d - 1.0
q_min = 0.5 - 0.5 * sqrt(1.0 - a * a)
I_max = 1.0 - H(q_min)
...
frontier_abs_dev_bits = abs(I_max(D) - I_emp)
```

With perfect kept-round recovery, `I_emp = 1`, so the frontier check enforces `I_max(D) >= 0.75`, which numerically means `D >= 0.150057...`. Combined with `MAX_QBER = 0.32`, successful transcripts must land in the window `0.150057... <= qber <= 0.32`. The exploit chain is therefore:

1. use the overpowered global unitary to turn every kept `info` round into exact secret recovery,
2. alternate announced bases to satisfy balance and keep enough rounds,
3. accept random `test`-round noise,
4. reconnect until the sampled `qber` falls inside the accepted window.

## Proof-of-Concept
- Step 1: Install a Bell-basis decoder on the full three-qubit register.

```python
ANCILLA = [1.0, 0.0]
# U = SWAP(a, c) * H(a) * CNOT(a -> b)
io.sendline(json.dumps(ANCILLA))
io.sendline(json.dumps(UNITARY))
```

This produces the decisive artifact: after the server applies `U`, `S0` states become `|001>` or `|010>`, and `S1` states become `(|001> +/- |010>) / sqrt(2)`. That converts the original hidden Bell encoding into measurements on `b` and `c` only.

- Step 2: Keep the basis announcements balanced while preserving enough `info` rounds.

```python
def choose_basis(counter: int) -> str:
    return "z" if counter % 2 == 0 else "x"

if round_type == "test":
    basis = choose_basis(test_counter)
    test_counter += 1
else:
    basis = choose_basis(info_counter)
    info_counter += 1

io.sendline(json.dumps({"basis": basis, "outcome": 0}))
```

This produces two useful artifacts. First, roughly half of `info` rounds satisfy `announce_basis == alice_basis` and are kept. Second, the score's balance checks on announced `z/x` bases and per-basis test coverage are satisfied.

- Step 3: Recover `secret` on kept `S0` information rounds.

```python
io.sendline(json.dumps([["b", "z"]]))
outcomes = io.recvline()
secret = int(outcomes.rsplit("=", 1)[1])
```

The returned artifact is a single measured bit `b:z`. Because the transformed states are `|001>` and `|010>`, this bit is exactly the hidden `secret`, so the next `secret_guess_bit` can be sent deterministically.

- Step 4: Recover `secret` on kept `S1` information rounds.

```python
io.sendline(json.dumps([["b", "x"], ["c", "x"]]))
parts = io.recvline().split()
b_bit = int(parts[1].rsplit("=", 1)[1])
c_bit = int(parts[2].rsplit("=", 1)[1])
secret = int(b_bit == c_bit)
```

The returned artifact is the pair `(b_x, c_x)`. Equal outcomes identify `(|01> + |10>) / sqrt(2)` and therefore `secret = 1`; different outcomes identify `(|01> - |10>) / sqrt(2)` and therefore `secret = 0`. This again yields an exact secret guess.

- Step 5: Retry until the noisy test statistics pass.

```python
for attempt in range(1, max_attempts + 1):
    flag = run_once(attempt)
    if flag is not None:
        print(flag)
        return
```

The artifact here is a successful transcript rather than a new primitive. Since `test` rounds are effectively guessed, some runs end with `No flag yet` or `ABORT`, but eventually one run lands in the acceptable `qber` window and the final line is the flag.

## P/S
The exploit is deterministic on kept `info` rounds; the only probabilistic part is the final score gate on `test` rounds. This is a protocol-design break, not a simulator bug: restricting the submitted unitary to Eve's ancilla-side interaction, or forcing the Bob/Charlie measurement plan to be committed before `public_set` is revealed, would remove the solve path used by `solve.py`.
