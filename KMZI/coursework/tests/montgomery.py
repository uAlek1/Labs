from sage.all import *
import curves.curves as curves
from curves.point import Point

#test swc -> mntg -> swc
E = curves.shortWeierstrassCurve(1337,1673, 1931)
print(E)
EE = E.toMontgomeryCurve()
P = Point(E, 56, 1057, 1)
print(P*6)
Q = P.toMontgomery(EE)
Q = Q*6
P = Q.toShortWeirstrass(E)
print(P)