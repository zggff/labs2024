import subprocess
from typing import List


def test(args: List[str], inp: str, out: str, status: int) -> bool:
    prog = ["./main.out"] + args
    p = subprocess.run(
        prog,
        text=True,
        input=inp,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if p.returncode != status:
        print(f"FAILURE: wrong return code: expected {status}, got {p.returncode}")
        return False
    if p.stdout.strip() != out.strip():
        print(f"FAILURE: wrong output: expected \n[{out}\n]\n, got \n[{p.stdout}\n]\n")
        return False

    test_leaks = ["leaks", "--atExit", "--list", "--"]
    prog = test_leaks + prog
    p = subprocess.run(
        prog,
        text=True,
        input=inp,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if p.returncode != 0:
        print(f"FAILURE: memory leaks")
        print(f"\n[{p.stdout}\n]")
        return False

    print(f"SUCCESS")
    return True
