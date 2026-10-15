"""Plot the deterministic solver CSV: python plotter.py trajectory.csv."""
import argparse
import numpy as np


def read_data_from_file(filepath):
    """Return finite t,x,y,z columns; reject malformed or unordered output."""
    with open(filepath) as stream:
        if stream.readline().strip() != "t,x,y,z":
            raise ValueError("Expected CSV header t,x,y,z")
    data = np.loadtxt(filepath, delimiter=",", skiprows=1, ndmin=2)
    if data.shape[1] != 4 or not len(data) or not np.isfinite(data).all():
        raise ValueError("Expected finite t,x,y,z rows")
    if np.any(np.diff(data[:, 0]) <= 0):
        raise ValueError("Times must be strictly increasing")
    return tuple(data[:, i] for i in range(4))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("csv")
    parser.add_argument("--output")
    args = parser.parse_args()
    t, x, y, z = read_data_from_file(args.csv)
    import matplotlib.pyplot as plt
    fig = plt.figure()
    ax = fig.add_subplot(111, projection="3d")
    ax.plot(x, y, z)
    ax.set(xlabel="x", ylabel="y", zlabel="z", title=f"Lorenz-63: t={t[0]:g} to {t[-1]:g}")
    if args.output:
        fig.savefig(args.output, dpi=160)
    else:
        plt.show()


if __name__ == "__main__":
    main()
