import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import callables_mod

class CallablesTestCase(unittest.TestCase):

    def test(self):
        callables_mod.f1()
        callables_mod.f2(2)
        callables_mod.f2(a=3)
        callables_mod.f3(4, 5)
        callables_mod.f3(6, b=7)
        callables_mod.f3(a=8, b=9)

if __name__ == "__main__":
    unittest.main()
