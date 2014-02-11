import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import bp3
import callables_mod

class CallablesTestCase(unittest.TestCase):

    def testNoOverloads(self):
        self.assertEqual(callables_mod.f1(), None)
        self.assertEqual(callables_mod.f2(2), 2+3)
        self.assertEqual(callables_mod.f2(a=3), 3+3)
        self.assertEqual(callables_mod.f3(4, 5), 20.0)
        self.assertEqual(callables_mod.f3(6, b=7), 6.0*7)
        self.assertEqual(callables_mod.f3(a=8, b=9), 8.0*9)

    def testSignatureError(self):
        self.assertRaises(bp3.SignatureError, callables_mod.f1, 5)  # too many positional args
        self.assertRaises(bp3.SignatureError, callables_mod.f3, 5)  # not enough positional args
        self.assertRaises(bp3.SignatureError, callables_mod.f2, 2, a=4)  # arg both keyword and positional
        self.assertRaises(bp3.SignatureError, callables_mod.f2, b=4)  # invalid keyword

    def testOverload(self):
        # these should be disambiguated just by the number of arguments
        self.assertEqual(callables_mod.f(), callables_mod.f1())
        self.assertEqual(callables_mod.f(2), callables_mod.f2(2))
        self.assertEqual(callables_mod.f(a=3), callables_mod.f2(a=3))
        # these should be disambiguated by the keyword argument names
        self.assertEqual(callables_mod.f(a=8, b=9), callables_mod.f3(a=8, b=9))
        self.assertEqual(callables_mod.f(c=8, b=9), callables_mod.f4(c=8, b=9))
        # this should be ambiguous
        self.assertRaises(bp3.OverloadResolutionError, callables_mod.f, 4, 5)
        self.assertRaises(bp3.OverloadResolutionError, callables_mod.f, 5, b=4)

    def testExceptions(self):
        self.assertRaises(bp3.UnknownError, callables_mod.f5, 0)

if __name__ == "__main__":
    unittest.main()
