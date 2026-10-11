# Repair validation — 2026-10-11

Historical base: `11020bc96423a5c9b2c3e67bc60975fa6f3df89f`.
Original source/plotter unchanged. Additive implementation, eight tests: **8/8 pass**.
Command: `python -m unittest discover -s tests -v`.
Local environment: Python3.12.13, SciPy1.18.0, NumPy2.5.2;
g++13.3.0; compilation uses C++17, O2, Wall, Wextra, Werror.
Elapsed test suite approximately0.52s. CI uses a separate declared dependency set;
its outcome must be read from GitHub rather than inferred from this local result.

Self-review by implementing assistant; no independent assessor. Tests establish
bounded software/numerical evidence only. No historical long trajectory was
regenerated; no neural data, FPE or assimilation qualification follows.
