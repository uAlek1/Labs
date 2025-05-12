import sys
import os
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))

import time

from sage.all import *

import curves.curves as curves
from curves.point import Point

from curves.ecp import sign, verify


def create_large_file(size_in_mb: int):
    import os

    MB1 = 1024 * 1024
    size = size_in_mb
    with open('bin/large_file', 'wb') as fout:
        for i in range(size):
            fout.write(os.urandom(MB1))


p = 0x00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97
a = 0x00C2173F1513981673AF4892C23035A27CE25E2013BF95AA33B22C656F277E7335
b = 0x295F9BAE7428ED9CCC20E7C359A9D41A22FCCD9108E17BF7BA9337A6F8AE9513
q = 0x400000000000000000000000000000000FD8CDDFC87B6635C115AF556C360C67
d = 1567

E = curves.shortWeierstrassCurve(a, b, p)
P = Point(E,
          0x0091E38443A5E82C0D880923425712B2BB658B9196932E02C78B2582FE742DAA28,
          0x32879423AB1A0375895786C4BB46E9565FDE0B5344766740AF268ADB32322E5C,
          1)
Q = P * d


def measure_sw():
    with open("bin/large_file", "rb") as f:
        m = f.read()

    start = time.time()
    s = sign(q, d, m, P)
    end = time.time()
    print(f"Signing took {end - start} seconds")

    start = time.time()
    verify(q, Q, P, m, s)
    end = time.time()
    print(f"Verifying took {end - start} seconds")


def measure_jacobi():
    with open("bin/large_file", "rb") as f:
        m = f.read()

    J = E.toJacobiCurve()
    PJ = P.toJacobi(J)
    QJ = Q.toJacobi(J)

    start = time.time()
    s = sign(q, d, m, PJ)
    end = time.time()
    print(f"Signing took {end - start} seconds")

    start = time.time()
    verify(q, QJ, PJ, m, s)
    end = time.time()
    print(f"Verifying took {end - start} seconds")


def main():
    # create_large_file(15)
    # measure_sw()
    measure_jacobi()
    return


if __name__ == "__main__":
    main()
