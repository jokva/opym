import unittest
import opym

class TestParseContext(unittest.TestCase):

    def test_constructor(self):
        opym.ParseContext()
        cta = ( "PARSE_RANDOM_SLASH", opym.InputErrorAction.ignore )
        ctl = [ cta ]
        ctx = opym.ParseContext( ctl )
        opym.ParseContext( ( ctl ) )
        opym.ParseContext( [("PARSE_RANDOM_SLASH", opym.InputErrorAction.ignore)] )

if __name__ == '__main__':
        unittest.main()
