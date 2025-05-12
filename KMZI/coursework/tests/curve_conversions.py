import unittest

from sage.all import *

import curves.curves as curves
from curves.point import Point


class Test_1(unittest.TestCase):
    def setUp(self):
        a = 18634552728166809231510976913427038192077900517328945994502918606685929703455781949403
        p = 30027634396791500130039852308813399949208517194801219053437836299004059473447384664181
        b = 2413823686514039444327367172680225478315761279952224311856000304789266644488059744875
        self.curve = curves.shortWeierstrassCurve(a, b, p)

    def test_to_montgomery(self):
        try:
            self.curve.toMontgomeryCurve()
        except Exception:
            self.fail("Failed to convert to Montgomery curve")

    def test_to_edwards(self):
        try:
            self.curve.toEdwardsCurve()
        except Exception:
            self.fail("Failed to convert to Edwards curve")

    def test_to_twisted_edwards(self):
        try:
            self.curve.toTwistedEdwardsCurve()
        except Exception:
            self.fail("Failed to convert to twisted Edwards curve")

    def test_to_hessian(self):
        try:
            self.curve.toHessianCurve()
        except Exception:
            self.fail("Failed to convert to Hessian curve")

    def test_to_jacobi(self):
        try:
            self.curve.toJacobiCurve()
        except Exception:
            self.fail("Failed to convert to Jacobi curve")

    def test_to_doche_icart_kohel(self):
        try:
            self.curve.toDocheIcartKohelCurve()
        except Exception:
            self.fail("Failed to convert to Doche Icart Kohel curve")


if __name__ == '__main__':
    unittest.main()
