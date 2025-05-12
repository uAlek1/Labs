from sage.all import *

import curves.curves as curves
from curves.point import Point

from gostcrypto import gosthash
import random
import time


def sign(q: int, d: int, m: bytes, P: Point):
    F = GF(q)

    start = time.time()
    e = F(int.from_bytes(gosthash.GOST34112012('streebog256', m).digest(), 'big'))
    end = time.time()
    print(f"Hash took {end - start} seconds")

    if (e == 0):
        e = 1
    while (1):
        k = random.randint(1, q)
        C = P * k
        C = C.curve._normalize(C)
        r = int(C.x)
        if (r != 0):
            s = F(r * d + k * e)
            if s != 0:
                s = int(s)
                break
    return (r, s)

def verify(q: int, Q: Point, P: Point, m: bytes, sign) -> bool:
    F = GF(q)
    start = time.time()
    e = F(int.from_bytes(gosthash.GOST34112012('streebog256', m).digest(), 'big'))
    end = time.time()
    print(f"Hash took {end - start} seconds")

    if (e == 0):
        e = 1
    v = F(1) / e
    C = P * int(F(sign[1] * v)) - Q * int(F(sign[0] * v))
    C = C.curve._normalize(C)
    if (C.x == sign[0]):
        return True
    return False
