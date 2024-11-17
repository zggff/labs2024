import os
import sys

sys.path.append(os.getcwd() + "/..")

from test_func import set_test_leaks, test

set_test_leaks(True)

expected = """\
2650
Perishable Products
name = "Perishable"
	id = "12"
	weight = 12.4
	price = 42
	in storage = 12
name = "Perishable2"
	id = "122"
	weight = 12.9
	price = 42
	in storage = 14

Electronic Products
name = "Electronic"
	id = "elec"
	weight = 12.4
	price = 42
	in storage = 12
	warranty period = 2
	power rating = 4

Building Materials
name = "Building"
	id = "cc"
	weight = 12.4
	price = 42
	in storage = 12

2130
Perishable Products
name = "Perishable"
	id = "12"
	weight = 12.4
	price = 42
	in storage = 12
name = "Perishable2"
	id = "122"
	weight = 12.9
	price = 42
	in storage = 14

Electronic Products
name = "Electronic"
	id = "elec"
	weight = 12.4
	price = 42
	in storage = 12
	warranty period = 2
	power rating = 4

Building Materials

expire after 1
expire after 10
name = "Perishable2"
	id = "122"
	weight = 12.9
	price = 42
	in storage = 14
expire after 30
name = "Perishable"
	id = "12"
	weight = 12.4
	price = 42
	in storage = 12
name = "Perishable2"
	id = "122"
	weight = 12.9
	price = 42
	in storage = 14
Warehouse["elec"] = {
	name = "Electronic"
	id = "elec"
	weight = 12.4
	price = 42
	in storage = 12
	warranty period = 2
	power rating = 4
}
"""

assert test("correct output", [], 0, "", expected)
