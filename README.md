# Lorenz-63 trajectory generation in C++

Research-engineering utility for deterministic Lorenz trajectories. This repository
does not solve a Fokker–Planck equation or implement data assimilation.

## Recommended implementation

`lorenz_rk4.cpp` uses sequential classical fourth-order Runge–Kutta, streams CSV
output with constant state memory, takes an explicit initial condition, and
refuses an existing output path. No random initialisation or OpenMP dependency.

```sh
g++ -std=c++17 -O2 -Wall -Wextra -Werror lorenz_rk4.cpp -o lorenz_rk4
./lorenz_rk4 10000 0.001 trajectory.csv 1 1 1
python3 -m unittest discover -s tests -v
```

Arguments: steps, timestep, fresh output path, x0, y0, z0; optionally sigma, rho,
beta. Defaults: (10,28,8/3). Output includes the initial state and exactly STEPS
updates, with columns `t,x,y,z`. Time starts at zero, final time is STEPS×DT.
The tests need a C++17 compiler and SciPy; the generator itself needs neither
Python nor SciPy. `plotter.py` reads this CSV; run `python plotter.py trajectory.csv`. Record the exact command, commit/compiler version and output hash
when using trajectories for an experiment. Concurrent writers to the same output
path are unsupported. A failed run's partial output must not be accepted.

## Historical implementation and audit (2026-10-11)

The pre-repair `RungeKutta.cpp` and `plotter.py` are available in Git history.
Both entry points were repaired on 15 October 2026, as documented below.
The original source attempts to parallelise sequentially dependent timesteps,
shares mutable state, refers to undeclared OpenMP private variables, writes the
initial state over the first update, and uses non-reproducible random initialisation.
Do not use its output as a qualified reference.

Method: classical RK4, engineering implementation of its explicit four-stage
formula. Tests check time/row accounting, equilibrium, exact exponential decay
on the invariant x=y=0 subspace, approximately fourth-order decay-error contraction,
invalid input, output preservation and a short nontrivial Lorenz forecast against
SciPy DOP853. Reference API/method attribution:
[SciPy solve_ivp](https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.solve_ivp.html).
DOP853 is an independent algorithm; agreement is numerical evidence, not proof
of long-time chaotic trajectory accuracy or attractor-law convergence. No new
benchmark speedup or scientific dataset qualification is claimed.

## Entry-point repairs — 15 October 2026
The legacy RungeKutta.cpp entry point now compiles the tested sequential solver. The plotter reads its CSV format, accepts a path and validates finite ordered data. Historical versions remain available in Git history.
