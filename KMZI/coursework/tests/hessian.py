from sage.all import *

import curves.curves as curves
from curves.point import Point

#test swc -> hessian -> swc
E = curves.shortWeierstrassCurve(1609,420, 1931)
P = Point(E, 333 , 1789, 1)
print(P*165)
EE = E.toHessianCurve()
Q = P.toHessian(EE)
Q = Q * 165
Q = Q.toShortWeirstrass(E)
print(Q)