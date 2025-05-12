from sage.all import *
from curves.point import Point


class EdwardsCurve(object):
    def __init__(self, c, d, p):
        self.field = GF(p)
        self.c = c
        self.d = d
        if self.field(c*d*(1-c**4*d)) == 0:
            raise Exception("incorrect parameters")

    def testPoint(self, x, y, z):
        # X^2*Z^2 + Y^2*Z^2 = c^2Z^4 +c^2*d*X^2*Y^2
        left = self.field(z**2 * (x**2 + y**2))
        right = self.field(self.c ** 2 * (z**4 + self.d * x**2 * y**2))
        return left == right

    def __str__(self):
        return f"x^2 + y^2 = {self.c}^2 * (1 + {self.d}x^2y^2)"

    def _neg(self, P):
        P = self._normalize(P)
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, -x, y, z)

    def _normalize(self, P):
        inv_z = self.field(1/P.z)
        return Point(P.curve, self.field(P.x*inv_z), self.field(P.y*inv_z), self.field(P.z*inv_z))

    def j_invariant(self):
        u = self.field(self.d * self.c**4)
        up = self.field(16 * (u**2 + 14*u + 1)**3)
        down = self.field(u*(u-1)**4)
        return up / down

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if x == 0 and y == self.c and z == 1:
            return True
        else:
            return False

    def _add(self, first: Point, second: Point):
        if (first.curve != second.curve):
            raise Exception("The points belong to different curves")
        if (first.is_zero()):
            return second
        if (second.is_zero()):
            return first
        A = self.field(first.z * second.z)
        B = self.field(A**2)
        C = self.field(first.x * second.x)
        D = self.field(first.y * second.y)
        E = self.field(self.d * C * D)
        F = self.field(B-E)
        G = self.field(B+E)
        X3 = self.field(A*F*((first.x+first.y)*(second.x+second.y)-C-D))
        Y3 = self.field(A*G*(D-C))
        Z3 = self.field(self.c*F*G)
        return Point(first.curve, X3, Y3, Z3)

    def _double(self, P: Point):
        B = self.field((P.x+P.y)**2)
        C = self.field(P.x ** 2)
        D = self.field(P.y ** 2)
        E = self.field(C+D)
        H = self.field((self.c*P.z)**2)
        J = self.field(E-2*H)
        X3 = self.field(self.c*(B-E)*J)
        Y3 = self.field(self.c*E*(C-D))
        Z3 = self.field(E*J)
        return Point(P.curve, X3, Y3, Z3)

    def _mul(self, n: int, P: Point):
        if n < 0:
            return -self * -n

        result = Point(self, 0, self.c, 1)
        temp = P
        while n != 0:
            if n & 1:
                result = result + temp
            temp = temp.double()
            n >>= 1
        return result

    def toTwistedEdwardsCurve(self):
        return twistedEdwardsCurve(1, self.field(self.d * self.c ** 4), self.field.characteristic())

    def _toTwistedEdwardsPoint(self, P: Point, curve):
        d = self.field(self.c**4 * self.d)
        if curve.a != 1 and curve.d != d:
            raise Exception("dont transform")
        x = self.field(P.x) / self.c
        y = self.field(P.y) / self.c
        return Point(curve, x, y, 1)

    def toMontgomeryCurve(self):
        E = self.toTwistedEdwardsCurve()
        return E.toMontgomeryCurve()

    def _toMontgomeryPoint(self, P: Point, curve):
        E = self.toTwistedEdwardsCurve()
        P = P.toTwistedEdwards(E)
        return P.toMontgomery(curve)

    def toShortWeierstrassCurve(self):
        E = self.toTwistedEdwardsCurve()
        return E.toShortWeierstrassCurve()

    def _toShortWeirstrassPoint(self, P: Point, curve):
        E = self.toTwistedEdwardsCurve()
        P = P.toTwistedEdwards(E)
        return P.toShortWeirstrass(curve)

    def toHessianCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toHessianCurve()

    def _toHessianPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toHessian(curve)

    def toJacobiCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toJacobiCurve()

    def toDocheIcartKohelCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toDocheIcartKohelCurve()

    def _toDocheIcartKohelPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toDocheIcartKohel(curve)


class twistedEdwardsCurve(object):
    def __init__(self, a, d, p):
        self.field = GF(p)
        self.a = a
        self.d = d
        if self.field(a*d*(1-d)) == 0:
            raise Exception("incorrect parameters")

    def testPoint(self, x, y, z):
        left = self.field(self.a)
        left = self.field((left * x**2 + y**2)*z**2)
        right = self.field(z**4 + self.d * x**2 * y**2)
        return left == right

    def __str__(self):
        return f"{self.a}x^2 + y^2 = 1 + {self.d}x^2y^2"

    def _neg(self, P):
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, -x, y, z)

    def _normalize(self, P):
        if (P.z == 0 or P.z == 1):
            return P

        inv_z = self.field(1/P.z)
        return Point(P.curve, self.field(P.x*inv_z), self.field(P.y*inv_z), self.field(P.z*inv_z))

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if x == 0 and y == 1 and z == 1:
            return True
        else:
            return False

    def j_invariant(self):
        up = self.field(
            16 * (self.a ** 2 + 14 * self.a * self.d + self.d ** 2)**3)
        down = self.field(self.a*self.d*(self.a-self.d)**4)
        return up / down

    def _add(self, first: Point, second: Point):
        if (first.curve != second.curve):
            raise Exception("The points belong to different curves")
        if (first.is_zero()):
            return second
        if (second.is_zero()):
            return first

        A = self.field(first.z * second.z)
        B = self.field(A ** 2)
        C = self.field(first.x * second.x)
        D = self.field(first.y * second.y)
        E = self.field(self.d * C * D)
        F = self.field(B - E)
        G = self.field(B + E)
        x3 = self.field(A * F * ((first.x + first.y) *
                        (second.x + second.y) - C - D))
        y3 = self.field(A * G * (D - self.a * C))
        z3 = self.field(F * G)
        return Point(first.curve, x3, y3, z3)

    def _double(self, P: Point):
        B = self.field((P.x + P.y) ** 2)
        C = self.field(P.x**2)
        D = self.field(P.y ** 2)
        E = self.field(self.a*C)
        F = self.field(E+D)
        H = self.field(P.z ** 2)
        J = self.field(F - 2*H)
        x3 = self.field((B-C-D) * J)
        y3 = self.field(F * (E-D))
        z3 = self.field(F * J)
        return Point(P.curve, x3, y3, z3)

    def _mul(self, n: int, P: Point):
        if n < 0:
            return -self * -n

        result = Point(self, 0, 1, 1)
        temp = P
        while n != 0:
            if n & 1:
                result = result + temp
            temp = temp.double()
            n >>= 1

        return result

    def toEdwardsCurve(self):
        if self.a == 1:
            return EdwardsCurve(1, self.d, self.field.characteristic())
        if not self.a.is_square():
            raise Exception("need quadratic field expansion")
        return EdwardsCurve(1, self.field(self.d) / self.a, self.field.characteristic())

    def _toEdwardsPoint(self, P: Point, curve):
        try:
            a = sqrt(self.field(self.a))
            x = self.field(P.x) / a
            return Point(curve, self.field(curve.c * x), self.field(curve.c * P.y), 1)
        except:
            print(a)
            raise Exception("dont transform point")

    def toMontgomeryCurve(self):
        A = self.field(2 * (self.a + self.d))
        A = A / (self.a - self.d)
        B = self.field(4)
        B = B / (self.a - self.d)
        return MontgomeryCurve(A, B, self.field.characteristic())

    def _toMontgomeryPoint(self, P: Point, curve):
        a = self.field(2 * (self.a + self.d)) / (self.a - self.d)
        b = self.field(4) / (self.a - self.d)
        if (curve.a != a and curve.b != b):
            raise Exception("dont transform point")

        if P.y == 1:
            raise Exception("y-coordinate = 1")
        if (P.x == 0):
            raise Exception("x-coordinate = 0")

        x = self.field(1+P.y) / (1-P.y)
        y = self.field(1+P.y) / ((1-P.y) * P.x)
        return Point(curve, x, y, 1)

    def toShortWeierstrassCurve(self):
        E = self.toMontgomeryCurve()
        return E.toShortWeierstrassCurve()

    def _toShortWeirstrassPoint(self, P: Point, curve):
        E = self.toMontgomeryCurve()
        P = P.toMontgomery(E)
        return P.toShortWeirstrass(curve)

    def toHessianCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toHessianCurve()

    def _toHessianPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toHessian(curve)

    def toJacobiCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toJacobiCurve()

    def toDocheIcartKohelCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toDocheIcartKohelCurve()

    def _toDocheIcartKohelPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toDocheIcartKohel(curve)


class MontgomeryCurve(object):
    def __init__(self, a, b, p):
        self.field = GF(p)
        self.a = a
        self.b = b
        self.inv_4 = inverse_mod(4, p)
        if self.field(b) == 0 or self.field(a) == 2 or self.field(a) == self.field(-2):
            raise Exception("incorrect parameters")

    def testPoint(self, x, y, z):
        # by**2 * z = x**3 + a*x**2 * z  + x * z**2
        left = self.field(self.b * y ** 2 * z)
        right = self.field(x**3 + self.a * x**2 * z + x*z**2)
        return left == right

    def __str__(self):
        return f"{self.b}y^2 = x^3 + {self.a}x^2 + x"

    def _neg(self, P):
        P = self._normalize(P)
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, x, -y, z)

    def _normalize(self, P):
        inv_z = self.field(1) / P.z
        return Point(P.curve, self.field(P.x * inv_z), self.field(P.y * inv_z), self.field(P.z * inv_z))

    def j_invariant(self):
        up = self.field(256 * (self.a**2 - 3)**3)
        down = self.field(self.a**2 - 4)
        return up / down

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if z == 0:
            return True
        else:
            return False

    def _add(self, P: Point, Q: Point):
        if (P.curve != Q.curve):
            raise Exception("The points belong to different curves")
        if (P.is_zero()):
            return Q
        if (Q.is_zero()):
            return P
        
        x1, y1, z1 = P.x, P.y, P.z
        x2, y2, z2 = Q.x, Q.y, Q.z

        u = self.field(y2*z1 - y1*z2)
        v = self.field(x2*z1 - x1*z2)

        x3 = self.field(self.b * u**2 *v *z1*z2 - (self.a * z1*z2 +x1*z2 +x2*z1)*v**3)
        y3 = self.field(-self.b * u**3 * z1*z2 + (self.a*z1*z2 +2*x1*z2 + x2*z1)*u*v**2 - y1*z2*v**3)
        z3 = self.field(v**3 * z1*z2)
        
        return Point(P.curve, x3, y3, z3)

    def _double(self, P: Point):
        return self._mul(2, P)

    def _ladder_double(self, R):
        x3 = self.field(((R[0]+R[1])**2)*((R[0]-R[1])**2))
        a24 = self.field((self.a+2)*self.inv_4)
        z3 = self.field((4*R[0]*R[1])*((R[0]-R[1])**2+a24*(4*R[0]*R[1])))
        return (x3, z3)

    def _ladder_addition(self, P, Q, D):
        v0 = self.field(P[0]+P[1])
        v1 = self.field(Q[0]-Q[1])
        v1 = self.field(v1*v0)
        v0 = self.field(P[0]-P[1])
        v2 = self.field(Q[0]+Q[1])
        v2 = self.field(v2*v0)
        v3 = self.field(v1+v2)
        v3 = self.field(v3**2)
        v4 = self.field(v1-v2)
        v4 = self.field(v4**2)
        x3 = self.field(D[1]*v3)
        z3 = self.field(D[0]*v4)
        return (x3, z3)

    def _mul(self, k: int, P: Point):
        R0 = (P.x, P.z)
        R1 = self._ladder_double(R0)
        D = (P.x, P.z)
        k = bin(k)
        for i in range(3, len(k)):
            if int(k[i]) == 0:
                R0, R1 = self._ladder_double(
                    R0), self._ladder_addition(R0, R1, D)
            else:
                R0, R1 = self._ladder_addition(
                    R0, R1, D), self._ladder_double(R1)
        return self._recover(P, R0, R1)

    def _recover(self, P: Point, R0, R1):
        P = self._normalize(P)
        v1 = self.field(P.x * R0[1])
        v2 = self.field(R0[0] + v1)
        v3 = self.field(R0[0] - v1)
        v3 = self.field(v3**2)
        v3 = self.field(v3 * R1[0])
        v1 = self.field(2*self.a*R0[1])
        v2 = self.field(v2+v1)
        v4 = self.field(P.x*R0[0])
        v4 = self.field(v4+R0[1])
        v2 = self.field(v2*v4)
        v1 = self.field(v1*R0[1])
        v2 = self.field(v2-v1)
        v2 = self.field(v2*R1[1])
        y = self.field(v2-v3)
        v1 = self.field(2*self.b*P.y)
        v1 = self.field(v1*R0[1])
        v1 = self.field(v1*R1[1])
        x = self.field(v1*R0[0])
        z = self.field(v1*R0[1])
        return Point(P.curve, x, y, z)

    def toShortWeierstrassCurve(self):
        a = self.field(self.a**2)
        a = 3 - a
        a = a / (3*self.b**2)
        b = self.field(-self.a**3 - self.a * a * 9*self.b**2)
        b = b / (27 * self.b ** 3)

        p = int(self.field.characteristic())
        return shortWeierstrassCurve(a, b, p)

    def _toShortWeirstrassPoint(self, P: Point, curve):
        a = self.field(3-self.a**2) / self.field(3*self.b**2)
        b = self.field(2*self.a**3 - 9*self.a) / self.field(27*self.b**3)

        if curve.a != a and self.b != b:
            raise Exception("dont transform point")

        x = self.field(P.x)/self.b + self.field(self.a)/(3*self.b)
        y = self.field(P.y) / self.b
        return Point(curve, x, y, 1)

    def toTwistedEdwardsCurve(self):
        a = self.field(self.a + 2)
        a = a / self.b

        d = self.field(self.a - 2)
        d = d / self.b
        return twistedEdwardsCurve(a, d, self.field.characteristic())

    def _toTwistedEdwardsPoint(self, P: point, curve):
        a = self.field(self.a + 2) / self.b
        d = self.field(self.a - 2) / self.b

        if curve.a != a and curve.d != d:
            raise Exception("dont transform point")

        x = self.field(P.x) / P.y
        y = self.field(P.x - 1) / (P.x + 1)
        return Point(curve, x, y, 1)

    def toEdwardsCurve(self):
        E = self.toTwistedEdwardsCurve()
        return E.toEdwardsCurve()

    def _toEdwardsPoint(self, P: Point, curve):
        E = self.toTwistedEdwardsCurve()
        P = P.toTwistedEdwards(E)
        return P.toEdwards(curve)

    def toHessianCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toHessianCurve()

    def _toHessianPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toHessian(curve)

    def toJacobiCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toJacobiCurve()

    def toDocheIcartKohelCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toDocheIcartKohelCurve()

    def _toDocheIcartKohelPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toDocheIcartKohel(curve)


class HessianCurve(object):
    def __init__(self, d, p):
        self.field = GF(p)
        self.d = d
        if self.field(d**3 - 1) == 0:
            raise Exception("incorrect parameters")

    def testPoint(self, x, y, z):
        # x^3 + y^3 + z^3 = 3dxyz
        left = self.field(x**3 + y**3 + z**3)
        right = self.field(3 * self.d * x * y * z)
        return left == right

    def __str__(self):
        return f"x^3 + y^3 + 1 = 3 * {self.d} * xy"

    def _neg(self, P):
        P = self._normalize(P)
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, y, x, z)

    def _normalize(self, P):
        if P.z == 0 or P.z == 1:
            return P
        inv_z = self.field(1/P.z)
        return Point(P.curve, self.field(P.x*inv_z), self.field(P.y*inv_z), self.field(P.z*inv_z))

    def j_invariant(self):
        # U * (U**3 + 216) / U**3 - 27
        U = self.field(self.d * 3)
        up = U * (U**3 + 216)
        down = U ** 3 - 27
        return (up / down) ** 3

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if x == 1 and y == -1 and z == 0:
            return True
        else:
            return False

    def _add(self, P: Point, Q: Point):
        if (P.curve != Q.curve):
            raise Exception("The points belong to different curves")
        if (P.is_zero()):
            return Q
        if (Q.is_zero()):
            return P

        Y1X2 = self.field(P.y*Q.x)
        Y1Y2 = self.field(P.y*Q.y)
        Z1Y2 = self.field(P.z*Q.y)
        Z1Z2 = self.field(P.z*Q.z)
        X1Z2 = self.field(P.x*Q.z)
        X1X2 = self.field(P.x*Q.x)
        X3 = self.field(Z1Z2*Z1Y2-X1X2*Y1X2)
        Y3 = self.field(Y1Y2*Y1X2-Z1Z2*X1Z2)
        Z3 = self.field(X1X2*X1Z2-Y1Y2*Z1Y2)
        return Point(P.curve, X3, Y3, Z3)

    def _double(self, P: Point):
        A = self.field(P.x**2)
        B = self.field(P.y*(P.x+P.y))
        C = self.field(A+B)
        D = self.field(P.z*(P.z+P.x))
        E = self.field(A+D)
        F = self.field(C*(P.x-P.y))
        G = self.field(E*(P.z-P.x))
        Z3 = self.field(F*P.z)
        Y3 = self.field(-(F+G)*P.x)
        X3 = self.field(G*P.y)
        return Point(P.curve, X3, Y3, Z3)

    def _mul(self, n: int, P: Point):
        if n < 0:
            return -self * -n

        result = Point(self, 1, -1, 0)
        temp = P
        while n != 0:
            if n & 1:
                result += temp
            temp = temp.double()
            n >>= 1
        return result

    def toShortWeierstrassCurve(self):
        # v^2 = u^3 - 27d(d^3+8)u + 54(d^6-20d^3-8)
        A = self.field(self.d * (self.d**3 + 8))
        A = -27 * A
        B = self.field(54 * (self.d ** 6 - 20 * self.d**3 - 8))
        return shortWeierstrassCurve(A, B, self.field.characteristic())

    def _toShortWeirstrassPoint(self, P: Point, curve):
        x = self.field(12 * (self.d**3 - 1))
        x = x / (self.d + P.x + P.y)
        x = x - 9*self.d**2

        y = self.field(36*(P.y - P.x) * (self.d**3 - 1))
        y = y / (self.d + P.x + P.y)
        return Point(curve, x, y, 1)

    def toEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toEdwardsCurve()

    def _toEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toEdwards(curve)

    def toTwistedEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toTwistedEdwardsCurve()

    def _toTwistedEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toTwistedEdwards(curve)

    def toMontgomeryCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toMontgomeryCurve()

    def _toMontgomeryPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toMontgomery(curve)

    def toJacobiCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toJacobiCurve()

    def toDocheIcartKohelCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toDocheIcartKohelCurve()

    def _toDocheIcartKohelPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toDocheIcartKohel(curve)


class shortWeierstrassCurve(object):
    def __init__(self, a, b, p):
        self.field = GF(p)
        self.a = a
        self.b = b
        self.discriminant = self.field(-16 * (4 * a*a*a + 27 * b * b))
        if self.discriminant == 0:
            raise Exception("The curve %s is not smooth!" % self)

    def testPoint(self, x, y, z):
        y1 = self.field(y*y*z)
        x1 = self.field(x*x*x + self.a * x * z*z + self.b*z*z*z)
        return x1 == y1

    def __str__(self):
        return f"y**2 = x**3 + {self.a}x + {self.b}"

    def _normalize(self, P):
        if (P.z == 1 or P.z == 0):
            return P

        inv_z = self.field(1/P.z)
        return Point(P.curve, self.field(P.x*inv_z), self.field(P.y*inv_z), self.field(P.z*inv_z))

    def _neg(self, P):
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, x, -y, z)

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if x == 0 and y == 1 and z == 0:
            return True
        return False

    def j_invariant(self):
        n = self.field(4*self.a**3 + 27*self.b**2)
        n = 1 / n
        return self.field(1728*4*self.a**3 * n)

    def _add(self, first: Point, second: Point):
        if (first.curve != second.curve):
            raise Exception("The points belong to different curves")

        if (first.is_zero()):
            return second
        if (second.is_zero()):
            return first

        U1 = self.field(second.y * first.z)
        U2 = self.field(first.y * second.z)
        V1 = self.field(second.x * first.z)
        V2 = self.field(first.x * second.z)
        if (V1 == V2):
            if (U1 != U2):
                return Point(first.curve, 0, 1, 0)
            else:
                return first.double()

        U = self.field(U1 - U2)
        V = self.field(V1 - V2)
        W = self.field(first.z*second.z)
        A = self.field(U**2*W - V**3 - 2*V**2*V2)
        X3 = self.field(V*A)
        Y3 = self.field(U*(V**2*V2 - A) - V**3*U2)
        Z3 = self.field(V**3*W)
        return Point(first.curve, X3, Y3, Z3)

    def _double(self, P: Point):
        if (P.y == 0):
            return Point(P.curve, 0, 1, 0)
        W = self.field(self.a * P.z**2 + 3*P.x**2)
        S = self.field(P.y*P.z)
        B = self.field(P.x*P.y*S)
        H = self.field(W**2 - 8*B)
        X = self.field(2*H*S)
        Y = self.field(W*(4*B - H) - 8*P.y**2*S**2)
        Z = self.field(8*S**3)
        return Point(P.curve, X, Y, Z)

    def _mul(self, n: int, P: Point):
        if n < 0:
            return -self * -n
        result = Point(self, 0, 1, 0)
        temp = P
        while n != 0:
            if n & 1:
                result += temp
            temp = temp.double()
            n >>= 1
        return result

    def toMontgomeryCurve(self):
        PR = PolynomialRing(GF(self.field.characteristic()), 'x')
        x = PR.gen()
        L = PR(x**3 + self.a * x + self.b)
        roots = L.roots()
        if not roots:
            raise Exception("not roots poly")
        for i in range(len(roots)):
            alpha = roots[i][0]
            try:
                s = sqrt(self.field(3*alpha**2 + self.a))
                s = 1 / s
                return MontgomeryCurve(self.field(s * alpha * 3), s, self.field.characteristic())
            except:
                continue
        raise Exception("not quadratic residue")

    def _toMontgomeryPoint(self, P: Point, curve: MontgomeryCurve):
        PR = PolynomialRing(GF(self.field.characteristic()), 'x')
        x = PR.gen()
        L = PR(x**3 + self.a * x + self.b)
        roots = L.roots()
        if not roots:
            raise Exception("dont transform point")
        for i in range(len(roots)):
            alpha = roots[i][0]
            try:
                s = sqrt(self.field(3*alpha**2 + self.a))
                s = 1 / s
                if (curve.a != 3 * alpha * s and curve.b != s):
                    continue
                x = s * (P.x-alpha)
                y = s*P.y
                return Point(curve, x, y, 1)
            except:
                continue
        raise Exception("dont transform point")

    def toEdwardsCurve(self):
        E = self.toMontgomeryCurve()
        return E.toEdwardsCurve()

    def _toEdwardsPoint(self, P: Point, curve):
        E = self.toTwistedEdwardsCurve()
        P = P.toTwistedEdwards(E)
        return P.toEdwards(curve)

    def toTwistedEdwardsCurve(self):
        E = self.toMontgomeryCurve()
        return E.toTwistedEdwardsCurve()

    def _toTwistedEdwardsPoint(self, P: Point, curve):
        E = self.toMontgomeryCurve()
        P = P.toMontgomery(E)
        return P.toTwistedEdwards(curve)

    def toHessianCurve(self):
        # v^2 = u^3 - 27d(d^3+8)u + 54(d^6-20d^3-8)
        PR = PolynomialRing(self.field, 'x')
        x = PR.gen()
        first = PR(27 * x**4 + 27 * 8 * x + self.a)
        roots1 = first.roots()
        second = PR(54 * x**6 - 54 * 20 * x**3 - 54 * 8 - self.b)
        roots2 = second.roots()
        res = list(set(roots1) & set(roots2))
        if not res:
            raise Exception("error")

        return HessianCurve(res[0][0], self.field.characteristic())

    def _toHessianPoint(self, P: Point, curve):
        x = self.field(36*(curve.d**3 - 1) - P.y)
        x = x / (6*(P.x + 9*curve.d**2))
        x = x - (self.field(curve.d) / 2)

        y = self.field(P.y + 36*(curve.d**3 - 1))
        y = y / (6*(P.x + 9*curve.d**2))
        y = y - (self.field(curve.d) / 2)
        return Point(curve, x, y, 1)

    def toJacobiCurve(self):
        return JacobiCurve(self)

    def _toJacobiPoint(self, P: Point, curve):
        if (P.y == 0):
            return Point(curve, 0, -1, 1)
        elif (P.is_zero()):
            return Point(curve, 0, 1, 1)
        else:
            x = self.field(2 * (P.x - curve.theta))
            y = self.field((2 * P.x + curve.theta) *
                           (P.x - curve.theta)**2 - P.y**2)
            z = self.field(P.y)
            return Point(curve, x, y, z)

    def toDocheIcartKohelCurve(self):
        PR = PolynomialRing(GF(self.field.characteristic()), 'x')
        x = PR.gen()
        L = PR(x**2 - 48 * x + 3 * self.a)
        roots1 = L.roots()
        L = PR(2 * x**3 - 144 * x**2 - 27 * self.b)
        roots2 = L.roots()
        res = list(set(roots1) & set(roots2))
        if not res:
            raise Exception("Can't transform")
        return DocheIcartKohelCurve(res[0][0], self.field.characteristic())

    def _toDocheIcartKohel(self, P: Point, curve):
        x = self.field(P.x - self.field(curve.a) / 3)
        return Point(curve, x, P.y, 1)

    def cardinality(self):
        E = EllipticCurve(GF(self.field.characteristic()), [self.a, self.b])
        return E.cardinality()


class JacobiCurve(object):
    def __init__(self, swc: shortWeierstrassCurve):
        # y^2 = e * x^4 - 2d * x^2 + 1
        # y^2 = e * x^4 - 2d * x^2 * z^2 + z^4
        self.field = GF(swc.field.characteristic())
        if self.field.characteristic() == 2:
            raise Exception("Field characteristic should not be == 2")

        PR = PolynomialRing(GF(swc.field.characteristic()), 'x')
        x = PR.gen()
        L = PR(x**3 + swc.a * x + swc.b)
        roots = L.roots()
        if not roots:
            raise Exception(
                "This Weierstrass Curve does not have a point of order 2")

        self.theta = roots[0][0]
        self.epsilon = self.field(-(3 * self.theta**2 + 4 * swc.a)) / 16
        self.delta = self.field((3 * self.theta)) / 4

        if self.field(self.delta**2 - 1) == 0:
            raise Exception("Delta should be different from +-1")

    def testPoint(self, x, y, z):
        left = self.field(y**2)
        right = self.field(self.epsilon * x**4)
        right -= 2 * self.delta * x**2 * z**2
        right += z**4
        return left == right

    def __str__(self):
        return f"y^2 = {self.epsilon} * x^4 - 2 * {self.delta} * x^2 + 1"

    def _neg(self, P):
        return Point(self, -P.x, P.y, P.z)

    def _normalize(self, P):
        # when is it actually zero?
        if P.z == 0:
            raise Exception(
                "There does not exist a point in affine coordinates")

        inv_z = self.field(1 / P.z)
        return Point(self, self.field(P.x * inv_z), self.field(P.y * inv_z**2), 1)

        # x = (self.field(2 * (P.y + P.z**2)) / P.x**2)- (self.field(self.theta) / 2)
        # y = self.field(P.z * (4 * (P.y + P.z**2) - 3 * self.theta * P.x**2)) / P.x**3
        # return Point(self, x, y, 1)
        return P

    def j_invariant(self):
        # 64 * (d**2 + 3e)**3 / e * (d**2 - e)**2
        up = self.field(64 * (self.delta**2 + 3 * self.epsilon)**3)
        down = self.field(self.epsilon * (self.delta**2 - self.epsilon)**2)
        return up / down

    def _is_zero(self, P):
        if (P.x, P.y, P.z) == (0, 1, 1):
            return true
        return false

    def _add(self, first: Point, second: Point):
        if (first.curve != second.curve):
            raise Exception("The points belong to different curves")

        x1, y1, z1 = first.x, first.y, first.z
        x2, y2, z2 = second.x, second.y, second.z
        e = self.epsilon
        d = self.delta

        x3 = self.field(x1 * z1 * y2 + y1 * x2 * z2)
        y3 = self.field((z1**2 * z2**2 + e * x1**2 * x2**2))
        y3 *= self.field((y1 * y2 - 2 * d * x1 * x2 * z1 * z2))
        y3 += self.field(2 * e * x1 * x2 * z1 * z2 *
                         (x1**2 * z2**2 + z1**2 * x2**2))
        z3 = self.field(z1**2 * z2**2 - e * x1**2 * x2**2)

        return Point(self, x3, y3, z3)

    def _double(self, P: Point):
        return self._add(P, P)

    def _mul(self, n: int, P: Point):
        result = Point(self, 0, 1, 1)
        temp = P
        while n != 0:
            if n & 1:
                result += temp
            temp = temp.double()
            n >>= 1
        return result

    def toShortWeierstrassCurve(self):
        # 2v^2 = u^3 - 2au^2 + (a^2-1)u
        # https://pure.tue.nl/ws/portalfiles/portal/67740154/775251-1.pdf
        # A = self.field(4 * (self.a ** 2 + 3))
        # A = A/3
        # A = -A

        # B = self.field(16 * self.a * (self.a-3) * (self.a+3))
        # B = B / 27

        # https://www-fourier.univ-grenoble-alpes.fr/mphell/doc-v4/conversion_weierstrass_jacobi.html
        a = self.field(-4 * (self.delta**2 / 3 + self.epsilon))
        b = self.field((64 * self.delta**3 + 36 * self.delta * a) / 27)
        return shortWeierstrassCurve(a, b, self.field.characteristic())

    def _toShortWeirstrassPoint(self, P: Point, curve):
        # P = self._normalize(P)
        if P.is_zero():
            return Point(curve, 0, 1, 0)
        elif P.x == 0 and P.y == -1 and P.z == 1:
            return Point(curve, self.theta, 0, 1)
        else:
            x = self.field(2 * (P.y + P.z**2)) / P.x**2
            x = x - self.field(self.theta) / 2
            y = self.field(P.z * (4 * (P.y + P.z**2) - 3 *
                           self.theta * P.x**2)) / P.x**3
            return Point(curve, x, y, 1)

    def toEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toEdwardsCurve()

    def _toEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toEdwards(curve)

    def toTwistedEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toTwistedEdwardsCurve()

    def _toTwistedEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toTwistedEdwards(curve)

    def toMontgomeryCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toMontgomeryCurve()

    def _toMontgomeryPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toMontgomery(curve)

    def toHessianCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toHessianCurve()

    def _toHessianPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toHessian(curve)

    def toDocheIcartKohelCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toDocheIcartKohelCurve()

    def _toDocheIcartKohelPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toDocheIcartKohel(curve)


class DocheIcartKohelCurve(object):
    def __init__(self, a, p):
        self.field = GF(p)
        self.a = a
        if (self.field(self.a * (self.a-64)) == 0):
            raise Exception("The curve %s is not smooth!" % self)

    def testPoint(self, x, y, z):
        y1 = self.field(y*y*z)
        x1 = self.field(x*x*x*z**2 + self.a * x**2 * z**3 + 16*self.a*x*z**4)
        return x1 == y1

    def __str__(self):
        return f"y^2 = x^3 + {self.a}x^2 + 16 * {self.a} * x"

    def _normalize(self, P):
        if (P.z != 0 and P.z != 1):
            inv_z = self.field(1)
            inv_z = inv_z / P.z
            inv_zz = self.field(1)
            inv_zz = inv_zz / P.zz
            return Point(P.curve, self.field(P.x*inv_z), self.field(P.y*inv_zz), 1)
        else:
            return P

    def _neg(self, P):
        x, y, z = P.x, P.y, P.z
        return Point(P.curve, x, -y, z)

    def _is_zero(self, P):
        x, y, z = P.x, P.y, P.z
        if x == 0 and y == 1 and z == 0:
            return True
        return False

    def j_invariant(self):
        n = self.field((self.a - 48)**3)
        n = n / (self.a - 64)
        return n

    def _add(self, first: Point, second: Point):
        if (first.curve != second.curve):
            raise Exception("The points belong to different curves")
        first = first.normalize()
        second = second.normalize()
        if (first.is_zero()):
            return second
        if (second.is_zero()):
            return first
        if (first == second):
            return self._double(first)

        A = self.field(second.y*first.zz - first.y)
        B = self.field(second.x*first.z - first.x)
        C = B * first.z
        z3 = C**2
        D = second.x * z3
        E = A**2
        F = first.x * B * C
        x3 = E - self.a * z3 - D - F
        G = z3**2
        H = A * C
        y3 = H * (D - x3) - second.y*G
        return Point(self, x3, y3, z3)

    def _double(self, P: Point):
        if (P.is_zero()):
            return P

        A = self.field(P.x ** 2)
        B = self.field(A - 16 * self.a * P.z**2)
        YT = self.field(P.y * B)
        x3 = self.field(B**2)
        z3 = self.field(4 * P.y ** 2)
        C = self.field(A * self.a * P.z**2)
        D = self.field(z3 ** 2)
        E = self.field(self.a * (z3 - 4 * C))
        y3 = self.field(YT * (2 * x3 + E + 256*C))
        return Point(self, x3, y3, z3)

    def _mul(self, n: int, P: Point):
        if n < 0:
            return -self * -n
        result = Point(self, 0, 1, 0)
        temp = P
        while n != 0:
            if n & 1:
                temp = temp.curve._normalize(temp)
                result += temp
            temp = temp.double()
            n >>= 1
        return result

    def toMontgomeryCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toMontgomeryCurve()

    def _toMontgomeryPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toMontgomery(curve)

    def toEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toEdwardsCurve()

    def _toEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toEdwards(curve)

    def toTwistedEdwardsCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toTwistedEdwardsCurve()

    def _toTwistedEdwardsPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toTwistedEdwards(curve)

    def toHessianCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toHessianCurve()

    def _toHessianPoint(self, P: Point, curve):
        E = self.toShortWeierstrassCurve()
        P = P.toShortWeirstrass(E)
        return P.toHessian(curve)

    def toJacobiCurve(self):
        E = self.toShortWeierstrassCurve()
        return E.toJacobiCurve()

    def toShortWeierstrassCurve(self):
        # в обобщенной форме Вейерштрасса, с коэф а1,а3, а6 = 0
        A = self.field(self.a * (48-self.a))
        A = A/3
        B = self.field(2*self.a**2 * (self.a - 72))
        B = B / 27
        return shortWeierstrassCurve(A, B, self.field.characteristic())

    def _toShortWeirstrassPoint(self, P: Point, curve):
        x = self.field(P.x)
        x = x + self.field(self.a) / 3
        return Point(curve, x, P.y, 1)
