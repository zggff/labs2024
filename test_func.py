"""
usage:
import sys
import os

sys.path.append(os.getcwd() + '/..')
"""

import subprocess
import sys
from typing import List, Optional

cnt = 0

def test(
    test_name: str,
    args: List[str],
    status: int,
    inp: str,
    out: str,
    ofile: Optional[str] = None,
    ofile_val: Optional[str] = None,
    prog_name="./main.out",
) -> bool:
    global cnt
    cnt += 1
    print(f"TEST {cnt:02}: {test_name}", end="\n\t")
    prog = [prog_name] + args
    p = None
    try:
        p = subprocess.run(
            prog,
            text=True,
            input=inp,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            encoding=sys.stdout.encoding,
        )
    except UnicodeDecodeError:
        print(f"FAILURE: non utf8 symbol in output")
        return False

    if p.returncode != status:
        print(f"FAILURE: return code: expected {status}, got {p.returncode}")
        return False

    if p.stdout.splitlines() != out.splitlines():
        print(f"FAILURE: wrong output")
        return cmp_string(p.stdout, out)

    if ofile is not None and ofile_val is not None:
        s = open(ofile).read()
        if s.splitlines() != ofile_val.splitlines():
            print(f"FAILURE: wrong file output")
            return cmp_string(s, ofile_val)

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


def cmp_string(s: str, s_exp: str) -> bool:
    txt = s.splitlines()
    exp = s_exp.splitlines()
    for i in range(0, min(len(txt), len(exp))):
        if txt[i] != exp[i]:
            print(f"\tdifference in line {i + 1}: [{txt[i]}] < [{exp[i]}]")
            return False
    if len(txt) > len(exp):
        print(f"\tdifference in line {len(exp) + 1} [{txt[len(exp)]}] < []")
    if len(txt) < len(exp):
        print(f"\tdifference in line {len(txt) + 1} [] < [{exp[len(txt)]}]")
    return False
