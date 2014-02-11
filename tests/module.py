import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import bp3

class ModuleTestCase(unittest.TestCase):
    """Test the bp3 Python module
    """

    def testWrapperErrors(self):
        self.assertTrue(issubclass(bp3.WrapperError, Exception))
        self.assertTrue(issubclass(bp3.OverloadResolutionError, bp3.WrapperError))
        self.assertTrue(issubclass(bp3.OverloadResolutionError, TypeError))
        self.assertTrue(issubclass(bp3.FromPythonTypeError, bp3.WrapperError))
        self.assertTrue(issubclass(bp3.FromPythonTypeError, TypeError))
        self.assertTrue(issubclass(bp3.UnknownError, bp3.WrapperError))

    def testStdExceptions(self):
        self.assertTrue(issubclass(bp3.std.exception, Exception))

if __name__ == "__main__":
    unittest.main()
