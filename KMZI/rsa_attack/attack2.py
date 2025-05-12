from Crypto.Util.number import long_to_bytes, bytes_to_long, getPrime
from math import log2, gcd, sqrt
import random
from decimal import Decimal

def bezout(a, b):
    x, xx, y, yy = 1, 0, 0, 1
    while b:
        q = a // b
        a, b = b, a % b
        x, xx = xx, x - xx * q
        y, yy = yy, y - yy * q
    return x

def generate_winer_params(bitlen : int):
    q = getPrime(bitlen)
    while 1:
        p = getPrime(bitlen)
        if(p > q and p < 2*q):
            break
    
    n, phi = p * q, (p-1) * (q-1)
    up = int(Decimal(n).sqrt().sqrt()) // 3
    d = random.randint(1, min(up, phi))
    while 1:
        while (gcd(d, phi) != 1):
            d = random.randint(1, min(up, phi))
        e = bezout(d,phi)
        if(gcd(e, phi) == 1):
            if(e<0):
                e = e + phi
            print(f"{e*d %phi = }")
            break
    
    return e,d,n

def cf_expansion(nm: int, dn:int, l:int) -> list:
    cf = []
    a, r = nm // dn, nm % dn
    cf.append(a)
    i = 0
    while r != 0 and i < l:
        nm, dn = dn, r
        a = nm // dn
        r = nm % dn
        cf.append(a)
        i+=1
    return cf

def Wiener_attack(n:int, e:int)->int:
    l = int(log2(n))
    x = cf_expansion(e, n, l)
    qq, pp, q, p = 0, 1, 1, 0
    mes = pow(11, e, n)
    for i in range(1, len(x)):
        qq, pp, q, p = q, p, q * x[i] + qq, p * x[i] + pp
        if(pow(mes, q, n) == 11):
            return q
        
    return -1

e, d, n = generate_winer_params(1024)
print(f"{e=}")
print(f"{d=}")
c = pow(bytes_to_long("Hello World".encode()), e, n)
wiener_d = Wiener_attack(n, e)
print(f"{wiener_d=}")
m = pow(c, d, n)
print("m =", long_to_bytes(m))