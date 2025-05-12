from sage.all import *

import curves.curves as curves
from curves.point import Point

E = curves.DocheIcartKohelCurve(5, 1931)
EE = E.toShortWeierstrassCurve()
print(E)
print(EE)
P = Point(EE, 107, 933, 1)
Q = Point(EE, 1400, 1801, 1)
print(f"P *15 = {P * 15}")
PP = P.toDocheIcartKohel(E)
QQQ = PP * 15
QQ = QQQ.toShortWeirstrass(EE)
print(f"P * 15 = {QQ}")

print(f"P + Q = {P + Q}")
QQ = Q.toDocheIcartKohel(E)
PP = P.toDocheIcartKohel(E)
QQQQ = QQ + PP
QQ = QQQQ.toShortWeirstrass(EE)
print(f"P + Q = {QQ}")
