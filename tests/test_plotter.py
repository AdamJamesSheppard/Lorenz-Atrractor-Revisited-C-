import tempfile
from pathlib import Path
import unittest
from plotter import read_data_from_file
class PlotterTests(unittest.TestCase):
    def test_csv(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/'data.csv';p.write_text('t,x,y,z\n0,1,2,3\n.1,2,3,4\n')
            self.assertEqual(read_data_from_file(p)[3].tolist(), [3,4])
    def test_invalid(self):
        for body in ['t,x,y,z\n0,1,2,nan\n', 't,x,y,z\n0,1,2,3\n0,2,3,4\n']:
            with tempfile.TemporaryDirectory() as d:
                p=Path(d)/'bad.csv';p.write_text(body)
                with self.assertRaises(ValueError): read_data_from_file(p)
