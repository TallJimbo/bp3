import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import callables_mod

class CallablesTestCase(unittest.TestCase):

    def test(self):
        self.assertEqual(callables_mod.f1(), None)
        self.assertEqual(callables_mod.f2(2), 2+3)
        self.assertEqual(callables_mod.f2(a=3), 3+3)
        self.assertEqual(callables_mod.f3(4, 5), 20.0)
        self.assertEqual(callables_mod.f3(6, b=7), 6.0*7)
        self.assertEqual(callables_mod.f3(a=8, b=9), 8.0*9)

if __name__ == "__main__":
    unittest.main()
