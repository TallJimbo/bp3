import unittest
import os
import sys

if os.path.exists(os.path.join(os.path.split(__file__)[0], "..", "python", "bp3")):
    sys.path.insert(0, os.path.abspath(os.path.join(os.path.split(__file__)[0], "..", "python")))

import bp3

class ModuleTestCase(unittest.TestCase):
    """Test the bp3 Python module
    """

    def testExceptions(self):
        self.assertTrue(issubclass(bp3.WrapperError, Exception))

if __name__ == "__main__":
    unittest.main()
