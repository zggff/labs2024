import os
import sys
import tempfile

sys.path.append(os.getcwd() + "/..")

from test_func import set_test_leaks, test

set_test_leaks(False)

assert test("no mode flag provided", [], 1, "", "ERROR: mode flag not provided")
assert test(
    "too many mode flags provided",
    ["--calculate", "--table"],
    1,
    "",
    "ERROR: only one mode flag can be provided",
)
assert test(
    "no input file provided", ["--calculate"], 1, "", "ERROR: input file not provided"
)

assert test(
    "failed to open file",
    ["not existing", "--table"],
    2,
    "",
    "ERROR: failed to open file: [not existing]",
)
