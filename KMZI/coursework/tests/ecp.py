import unittest

from sage.all import *

import curves.curves as curves
from curves.point import Point

from curves.ecp import sign, verify


class TestECP(unittest.TestCase):

    def setUp(self):
        p = 0x00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97
        a = 0x00C2173F1513981673AF4892C23035A27CE25E2013BF95AA33B22C656F277E7335
        b = 0x295F9BAE7428ED9CCC20E7C359A9D41A22FCCD9108E17BF7BA9337A6F8AE9513

        self.E_1 = curves.shortWeierstrassCurve(a, b, p)
        self.P_1 = Point(self.E_1,
                  0x0091E38443A5E82C0D880923425712B2BB658B9196932E02C78B2582FE742DAA28,
                  0x32879423AB1A0375895786C4BB46E9565FDE0B5344766740AF268ADB32322E5C,
                  1)
        self.q_1 = 0x400000000000000000000000000000000FD8CDDFC87B6635C115AF556C360C67

        p = 0x8000000000000000000000000000000000000000000000000000000000000431
        a = 0x7
        b = 0x5FBFF498AA938CE739B8E022FBAFEF40563F6E6A3472FC2A514C0CE9DAE23B7E

        self.E_2 = curves.shortWeierstrassCurve(a, b, p)
        self.P_2 = Point(self.E_2,
                  0x2,
                  0x8E2A8A0E65147D4BD6316030E16D19C85C97F0A9CA267122B96ABBCEA7E8FC8,
                  1)
        self.q_2 = 0x8000000000000000000000000000000150FE8A1892976154C59CFC193ACCF5B3

        p = 0x00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97
        a = 115792089237316195423570985008687907853269984665640564039457584007913126116548
        b = 2533069422

        self.E_3 = curves.shortWeierstrassCurve(a, b, p)
        self.E_3_hesse = curves.HessianCurve(19, p)
        self.P_3 = Point(self.E_3,
                         39927402077268388015688368373964060454167172856182858633706842200496292855392,
                         52671734590050214460475832011994697463205827662280651322383872992998053447546,
                         1)
        self.q_3 = 3216446923258783206210305139130219662589836859832544035189707273818837420569

        p = 1302730114318879697404365712556737649124670595592476151956615643886250761887619310033
        a = 8

        self.E_4 = curves.DocheIcartKohelCurve(a, p)
        self.P_4 = Point(self.E_4,
                         14198742606561531674924925771560196139900728605596891319039754224760595531237348556,
                         997621458940793897753905388530719402535658761188371642587162017500384759516364280323,
                         1)
        self.q_4 = 309290150598024619516706009628855092384774412735815962717972161929552795026274293

    def test_1(self):
        P = self.P_1
        q = self.q_1
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_1_montgomery(self):
        M = self.E_1.toMontgomeryCurve()
        P = self.P_1.toMontgomery(M)
        q = self.q_1
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_1_edwards(self):
        M = self.E_1.toEdwardsCurve()
        P = self.P_1.toEdwards(M)
        q = self.q_1
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_1_twisted_edwards(self):
        M = self.E_1.toTwistedEdwardsCurve()
        P = self.P_1.toTwistedEdwards(M)
        q = self.q_1
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_1_jacobi(self):
        M = self.E_1.toJacobiCurve()
        P = self.P_1.toJacobi(M)
        q = self.q_1
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, 1567, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_2(self):
        """
            This curve does not have a point of order 2 so can't
            transform it to Jacobi form
        """
        P = self.P_2
        q = self.q_2
        d = 0x7A929ADE789BB9BE10ED359DD39A72C11B60961F49397EEE1D19CE9891EC3B28
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_3(self):
        P = self.P_3
        q = self.q_3
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_3_hesse(self):
        M = self.E_3.toHessianCurve()
        P = self.P_3.toHessian(M)
        q = self.q_3
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_3_hesse_conv(self):
        E = self.E_3_hesse.toShortWeierstrassCurve()
        print(E, E.cardinality())

        E = self.E_3.toMontgomeryCurve()
        E = self.E_3.toEdwardsCurve()
        E = self.E_3.toDocheIcartKohelCurve()

    def test_4(self):
        P = self.P_4
        q = self.q_4
        d = 1567
        Q = P * d

        m = "Hello, world!".encode()
        s = sign(q, d, m, P)

        self.assertTrue(verify(q, Q, P, m, s))

    def test_4_doche_conv(self):
        E = self.E_4.toShortWeierstrassCurve()
        E = self.E_4.toMontgomeryCurve()
        E = self.E_4.toTwistedEdwardsCurve()
        # E = self.E_4.toEdwardsCurve()
        # E = self.E_4.toHessianCurve()


if __name__ == '__main__':
    unittest.main()
