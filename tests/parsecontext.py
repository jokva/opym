import unittest
import opym

class TestParseContext(unittest.TestCase):

    def test_constructor(self):
        cta = ( "PARSE_RANDOM_SLASH", opym.InputErrorAction.ignore )
        ctx = opym.ParseContext( [ cta ] )

if __name__ == '__main__':
        unittest.main()
