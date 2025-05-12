from Crypto.Util.number import getPrime
from math import gcd
import random

def gen_key(p, q, bits):
    while True:
        e = getPrime(bits//2)
        n, phi = p * q, (p - 1) * (q - 1)
        while (gcd(e, phi) != 1):
            e = getPrime(36)
        d = bezout(e, phi)
        if(d < 0):
            d = phi + d
        break

    return e,d

def bezout(a, b):
    x, xx, y, yy = 1, 0, 0, 1
    while b:
        q = a // b
        a, b = b, a % b
        x, xx = xx, x - xx * q
        y, yy = yy, y - yy * q
    return x

def common_modulus_attack(n, e1, d1, e2):
    razn = (e1 * d1) - 1
    while not (razn & 1):
        razn = razn >> 1

    while 1:
        a = random.randint(0, n)
        b = pow(a, razn, n)
        pred = 1
        l = 2
        while b != 1:
            pred = b
            b = pow(b, l, n)
            l *= 2
        if (pred != 1 and pred != n-1):
            break

    p = gcd(pred + 1, n)
    q = gcd(pred - 1, n)
    d = bezout(e2, (p-1)*(q-1))
    if(d<0):
        d = (p-1)*(q-1) + d
    return  p, q, d


p,q = getPrime(1024), getPrime(1024)
e1,d1 = gen_key(p,q,1024)
e2,d2 = gen_key(p,q,1024)

print(f"e1 = {e1}, d1 = {d1}")
print(f"e2 = {e2}, d2 = {d2}")

new_p,new_q,d = common_modulus_attack(p*q, e1,d1,e2)
print(f"{d=}")

if(d == d2):
    print("Success")
else:
    print("Unluck")

