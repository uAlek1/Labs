from sage.all import *
import curves.curves as curves
from curves.point import Point

#test swc->tec->ec->tec->sc
EEE = curves.EdwardsCurve(5, 3, 1931)
E = EEE.toShortWeierstrassCurve()
EE = E.toTwistedEdwardsCurve()
P = Point(E, 1514 , 888, 1)
print(P*7)
Q = P.toTwistedEdwards(EE)
Q = Q * 7
Q = Q.toShortWeirstrass(E)
print(Q)
Q = P.toEdwards(EEE)
Q = Q * 7
Q = Q.toShortWeirstrass(E)
print(Q)