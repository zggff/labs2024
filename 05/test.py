import os
import sys
import tempfile

sys.path.append(os.getcwd() + "/..")

from test_func import set_test_leaks, test

set_test_leaks(True)

assert test("input file not provided", [], 1, "", "ERROR: input file not provided")
assert test(
    "non existent file",
    ["non existent"],
    1,
    "",
    "ERROR: failed to open file [non existent]",
)

fp = tempfile.NamedTemporaryFile()
fp.writelines(
    [
        b"int *c[10][4];\n",
        b"char ***d[10];\n",
        b"int (*e[2])(int(**));\n"
        b"double f();\n"
        b"type c;\n"
        b"[ c;\n"
    ])
fp.seek(0)

assert test(
    "correct work",
    [fp.name],
    0,
    "",
    [
        "declare c as array of 10 elements of array of 4 elements of pointer to int",
        "declare d as array of 10 elements of pointer to pointer to pointer to char",
        "declare e as array of 2 elements of pointer to function returning int",
        "declare f as function returning double",
        "Invalid type: 'type'",
        "Syntax error in '[ c;' at position 0",
    ],
)
fp.close()
