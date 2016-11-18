import unittest
import opym

def name(x): return x.name

ignore_slash = ("PARSE_RANDOM_SLASH", opym.InputErrorAction.ignore)
norne_path = "../../opm-data/norne/NORNE_ATW2013.DATA"
parsed_norne = opym.parse(norne_path, [ignore_slash])

class TestEclipseState(unittest.TestCase):

    def setUp(self):
        self.ignore_slash = ("PARSE_RANDOM_SLASH",
                              opym.InputErrorAction.ignore)

        self.parse_context = opym.ParseContext([self.ignore_slash])
        self.norne = "../../opm-data/norne/NORNE_ATW2013.DATA";
        self.ecl = parsed_norne

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

        wells = sorted(map(name, sch.wells))
        self.assertEqual("B-1AH", wells[0])
        self.assertEqual("B-1BH", wells[1])
        self.assertEqual("K-3H", wells[-1])

    def test_well_properties(self):
        b2h = self.ecl.schedule["B-2H"]
        c4h = self.ecl.schedule["C-4H"]

        self.assertAlmostEqual(2627.8429, b2h.ref_depth, places = 4)
        self.assertAlmostEqual(2542.8729, c4h.ref_depth, places = 4)

        self.assertFalse(b2h.is_injector(0))
        self.assertTrue(b2h.is_producer(0))
        e2h = self.ecl.schedule["E-2H"]
        self.assertTrue(e2h.is_injector(0))

    def test_3D_properties(self):
        e3d = self.ecl.e3d
        self.assertTrue("SATNUM" in e3d)
        self.assertTrue("ACTNUM" in e3d)
        self.assertTrue("PORO" in e3d)
        self.assertFalse("PORV" not in e3d)


if __name__ == '__main__':
        unittest.main()

