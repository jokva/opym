import unittest
import opym

class TestEclipseState(unittest.TestCase):

    def setUp(self):
        self.ignore_slash = ("PARSE_RANDOM_SLASH",
                              opym.InputErrorAction.ignore)

        self.parse_context = opym.ParseContext([self.ignore_slash])
        self.norne = "../../opm-data/norne/NORNE_ATW2013.DATA";
        self.ecl = opym.parse(self.norne, [self.ignore_slash])

    def test_constructor(self):
        opym.parse(self.norne, self.parse_context)

    def test_properties(self):
        self.assertEqual("", self.ecl.title)
        rst = self.ecl.restart
        self.assertEqual(0, rst.first_step)
        self.assertEqual(True, rst.write(0))
        self.assertEqual(False, rst.write(6))

    def test_schedule(self):
        sch = self.ecl.schedule
        self.assertEqual(36, sch.num_wells())
        self.assertEqual(3, sch.num_wells(0))
        self.assertTrue(sch.has_well( "C-4H" ))
        self.assertTrue(sch.has_well( "B-2H" ))
        self.assertTrue("B-2H" in sch)
        self.assertFalse(sch.has_well( "dummy" ))

    def test_well(self):
        sch = self.ecl.schedule
        c4h = sch["C-4H"]
        b2h = sch["B-2H"]
        self.assertEqual("C-4H", c4h.name)
        self.assertEqual("B-2H", b2h.name)
        self.assertEqual(10, c4h.I)
        self.assertEqual(34, c4h.J)
        self.assertEqual(14, b2h.I)
        self.assertEqual(30, b2h.J)


if __name__ == '__main__':
        unittest.main()

