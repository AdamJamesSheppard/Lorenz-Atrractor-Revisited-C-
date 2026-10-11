import csv
import math
from pathlib import Path
import subprocess
import tempfile
import unittest


class IntegratorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory()
        cls.root = Path(cls.temp.name)
        cls.binary = cls.root / "rk4"
        subprocess.run(["g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-Werror",
                        str(Path(__file__).resolve().parents[1] / "lorenz_rk4.cpp"),
                        "-o", str(cls.binary)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def generate(self, steps, dt, state=(1, 1, 1), params=()):
        output = self.root / (self.id().split('.')[-1] + f"-{steps}-{dt}.csv")
        command = [str(self.binary), str(steps), str(dt), str(output),
                   *map(str, state), *map(str, params)]
        subprocess.run(command, check=True, capture_output=True)
        with output.open() as stream:
            rows = [{k: float(v) for k, v in r.items()} for r in csv.DictReader(stream)]
        return rows, command, output

    def test_initial_and_final_times(self):
        rows, _, _ = self.generate(10, 0.01)
        self.assertEqual(len(rows), 11)
        self.assertEqual(rows[0], dict(t=0, x=1, y=1, z=1))
        self.assertAlmostEqual(rows[-1]['t'], 0.1)

    def test_origin_equilibrium(self):
        rows, _, _ = self.generate(20, .01, (0, 0, 0))
        self.assertTrue(all(r['x'] == r['y'] == r['z'] == 0 for r in rows))

    def test_zero_steps(self):
        rows, _, _ = self.generate(0, .01)
        self.assertEqual(len(rows), 1)

    def test_fourth_order_against_exact_decay(self):
        errors = []
        for n in (10, 20, 40):
            rows, _, _ = self.generate(n, 1/n, (0, 0, 1), (10, 28, 1))
            errors.append(abs(rows[-1]['z'] - math.exp(-1)))
        for a, b in zip(errors, errors[1:]):
            self.assertGreater(a/b, 15)
            self.assertLess(a/b, 18)

    def test_no_overwrite(self):
        _, command, output = self.generate(1, .01)
        before = output.read_bytes()
        self.assertNotEqual(subprocess.run(command, capture_output=True).returncode, 0)
        self.assertEqual(before, output.read_bytes())

    def test_reproducible_explicit_initial_condition(self):
        _, command, first = self.generate(20, .01)
        second = self.root / 'repeat.csv'
        command[3] = str(second)
        subprocess.run(command, check=True, capture_output=True)
        self.assertEqual(first.read_bytes(), second.read_bytes())

    def test_invalid_arguments(self):
        for n, dt in [('1', '0'), ('-1', '.01'), ('1', 'nan'), ('1.5', '.01')]:
            result = subprocess.run([str(self.binary), n, dt, str(self.root/'invalid'),
                                     '1', '1', '1'], capture_output=True)
            self.assertNotEqual(result.returncode, 0)

    def test_short_horizon_independent_solver(self):
        from scipy.integrate import solve_ivp
        rows, _, _ = self.generate(100, .001)
        reference = solve_ivp(lambda t, u: [10*(u[1]-u[0]),
                              u[0]*(28-u[2])-u[1], u[0]*u[1]-8*u[2]/3],
                              (0, .1), (1, 1, 1), method='DOP853',
                              rtol=1e-12, atol=1e-14)
        self.assertTrue(reference.success)
        error = max(abs(rows[-1][k]-v) for k, v in zip(('x','y','z'), reference.y[:, -1]))
        self.assertLess(error, 1e-8)


if __name__ == '__main__':
    unittest.main()
