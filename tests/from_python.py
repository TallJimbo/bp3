import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import from_python_mod

class from_python_testcase(unittest.TestCase):

    def test(self):
        for suffix in ("val", "cval", "ref", "cref", "ptr", "cptr", "ptrc", "cptrc"):
            method = getattr(from_python_mod, "check_" + suffix)
            self.assertTrue(method(from_python_mod.py1))

if __name__ == "__main__":
    unittest.main()
