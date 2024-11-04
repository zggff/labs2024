import os
import sys
import tempfile

sys.path.append(os.getcwd() + "/..")

from test_func import set_test_leaks, test, run_process

set_test_leaks(False)

assert test("no input file provided", [], 1, "", "ERROR: input file not provided")

assert test(
    "no input file provided",
    ["not existing"],
    1,
    "",
    "ERROR: failed to open file: [not existing]",
)

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read"])
fp.seek(0)
assert test(
    "parse not enough tokens",
    [fp.name],
    2,
    "",
    ["ERROR: not enough tokens:", "\t{[read]}"],
)
fp.close()

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read(a, b, c);"])
fp.seek(0)
assert test(
    "parse too many tokens",
    [fp.name],
    2,
    "",
    ["ERROR: too many tokens:", "\t{[read] [(] [a] [,] [b] [,] [c] [)] [;]}"],
)
fp.close()

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read a, 12);"])
fp.seek(0)
assert test(
    "parse invalid format",
    [fp.name],
    2,
    "",
    ["ERROR: expected [(] got [a]"],
)
fp.close()

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read(a, 39);"])
fp.seek(0)
assert test(
    "parse invalid base",
    [fp.name],
    2,
    "",
    ["ERROR: base [39] not in range [2..36]"],
)
fp.close()

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read(a, 16);"])
fp.seek(0)
assert test(
    "read invalid",
    [fp.name],
    2,
    "fz",
    ["ERROR: failed to parse [fz] as number in base [16]"],
)
fp.close()

fp = tempfile.NamedTemporaryFile()
fp.writelines([b"read(a, 16);", b"write(a, 10);"])
fp.seek(0)
assert test(
    "read and write valid",
    [fp.name],
    0,
    "ff",
    ["A_10 = 255"],
)
fp.close()
