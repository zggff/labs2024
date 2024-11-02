import os
import sys

sys.path.append(os.getcwd() + "/..")

from test_func import test

test("no input file provided", [], 1, "", "ERROR: input file not provided")

test(
    "no input file provided",
    ["not existing"],
    1,
    "",
    'ERROR: failed to open file: "not existing"',
)

test(
    "successfull output into stdout",
    ["./inp.txt"],
    0,
    "",
    "hello, my name is Max Giga, and i am 19 years old.\neight in binary is 100",
)

test(
    "successfull output into file",
    ["./inp.txt", "./out.txt"],
    0,
    "",
    "",
    "./out.txt"
    "hello, my name is Max Giga, and i am 19 years old.\neight in binary is 100",
)