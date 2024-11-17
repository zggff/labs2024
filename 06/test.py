import os
import sys

sys.path.append(os.getcwd() + "/..")

from test_func import set_test_leaks, test

set_test_leaks(False)

expected = """\
initialization
size + element  = {
	cap = 10
	size = 10
	{4 4 4 4 4 4 4 4 4 4 }
}
size = {
	cap = 10
	size = 10
	{0 0 0 0 0 0 0 0 0 0 }
}
list = {
	cap = 6
	size = 6
	{0 12 -41.3 12 4 5 }
}
iter = {
	cap = 3
	size = 3
	{-41.3 12 4 }
}
reserve + shrink
list = {
	cap = 6
	size = 6
	{0 12 -41.3 12 4 5 }
}
reserved = {
	cap = 24
	size = 6
	{0 12 -41.3 12 4 5 }
}
shrunk = {
	cap = 6
	size = 6
	{0 12 -41.3 12 4 5 }
}
modify
list = {
	cap = 6
	size = 6
	{0 12 -41.3 12 4 5 }
}
added = {
	cap = 8
	size = 8
	{0 12 3.14 -41.3 12 3.1415 4 5 }
}
erased = {
	cap = 8
	size = 6
	{0 12 3.14 -41.3 3.1415 4 }
}
resized = {
	cap = 12
	size = 12
	{0 12 3.14 -41.3 3.1415 4 0.001 0.001 0.001 0.001 0.001 0.001 }
}
equality
a = {
	cap = 3
	size = 3
	{0 12 4 }
}
b = {
	cap = 4
	size = 4
	{0 12 4 4 }
}
c = {
	cap = 3
	size = 3
	{0 12 2 }
}
a <=> a = equal
a <=> b = less
a <=> c = greater
"""

assert test("correct output", [], 0, "", expected)
