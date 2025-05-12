import unittest

from sage.all import *

import curves.curves as curves
from curves.point import Point


class test_jacobi_curve(unittest.TestCase):

    E = curves.shortWeierstrassCurve(1337, 1673, 1931)

    def test_j_inv_equality_after_transform(self):
        E = self.E
        JC = self.E.toJacobiCurve()

        print(f"SW: {E}, with j-inv: {E.j_invariant()}")
        print(f"JC: {JC}, with j-inv: {JC.j_invariant()}")

        self.assertEqual(E.j_invariant(), JC.j_invariant())

    def test_point_conversion(self):
        E = self.E
        JC = self.E.toJacobiCurve()

        P = Point(E, 813, 1742, 1)
        PJC = P.toJacobi(JC)

        print(f"SW Point: {P}, to JC Point: {PJC}")

        self.assertTrue(E.testPoint(P.x, P.y, P.z))
        self.assertTrue(JC.testPoint(PJC.x, PJC.y, PJC.z))

    def test_point_mul_and_conversion(self):
        E = self.E
        JC = self.E.toJacobiCurve()

        P = Point(E, 813, 1742, 1)
        PJC = P.toJacobi(JC)

        P3 = P * 3
        PJC3 = PJC * 3

        PJC3_back = PJC3.toShortWeirstrass(E)

        print(f"P * 3 == {P3}, PJC * 3 == {PJC3}, PJC * 3 back to SW: {PJC3_back}")

        self.assertEqual(PJC3_back, P3)

    def test_zero_points(self):
        E = self.E
        JC = self.E.toJacobiCurve()

        zero = Point(E, 0, 1, 0)
        zero_jc = zero.toJacobi(JC)

        self.assertTrue(zero.is_zero())
        self.assertTrue(zero_jc.is_zero())

if __name__ == "__main__":
    unittest.main()
