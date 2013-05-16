#!/usr/bin/env python

import unittest
import from_python_test_mod

class from_python_testcase(unittest.TestCase):

    def test(self):
        for suffix in ("val", "cval", "ref", "cref", "ptr", "cptr", "ptrc", "cptrc"):
            method = getattr(from_python_test_mod, "check_" + suffix)
            self.assertTrue(method(from_python_test_mod.py1))

if __name__ == "__main__":
    unittest.main()
