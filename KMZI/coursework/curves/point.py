from sage.all import *


class Point(object):
    def __init__(self, curve, x: int, y: int, z: int):
        self.curve = curve
        self.x = x
        self.y = y
        self.z = z
        self.zz = self.curve.field(z**2)
        if not curve.testPoint(self.x, self.y, self.z):
            raise Exception(
                "The point %s is not on the given curve %s" % (self, curve))

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def __neg__(self):
        return self.curve._neg(self)

    def __add__(self, Q):
        return self.curve._add(self, Q)

    def __sub__(self, Q):
        return self + -Q

    def __mul__(self, n: int):
        return self.curve._mul(n, self)

    def __eq__(self, other):
        if isinstance(other, Point):
            P = self.curve._normalize(
                Point(self.curve, self.x, self.y, self.z))
            Q = other.curve._normalize(
                Point(other.curve, other.x, other.y, other.z))
            return (P.x, P.y, P.z) == (Q.x, Q.y, Q.z)
        return False

    def double(self):
        return self.curve._double(self)

    def is_zero(self):
        P = self.curve._normalize(self)
        return self.curve._is_zero(P)

    def normalize(self):
        return self.curve._normalize(self)

    def toMontgomery(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toMontgomeryPoint(P, curve)

    def toShortWeirstrass(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toShortWeirstrassPoint(P, curve)

    def toTwistedEdwards(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toTwistedEdwardsPoint(P, curve)

    def toEdwards(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toEdwardsPoint(P, curve)

    def toHessian(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toHessianPoint(P, curve)

    def toDocheIcartKohel(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toDocheIcartKohel(P, curve)

    def toJacobi(self, curve):
        P = self.curve._normalize(Point(self.curve, self.x, self.y, self.z))
        return self.curve._toJacobiPoint(P, curve)
