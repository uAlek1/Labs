from sage.all import *
import socket
from random import randint
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
from Crypto.Util.number import bytes_to_long, inverse
from sympy import legendre_symbol
from asn1mo import *

def f(message: bytes, p: int, a:int, b:int, curve, r)->tuple:
    F = GF(p)
    x = bytes_to_long(message)
    ert = True
    if (x > p):
        ert = False
    count = 0
    while(count < 255):
        if(legendre_symbol(int(F(x**3 + a*x + b)), p) == 1):
            break
        x+=1
        count+=1
    if(count == 255):
           raise Exception("This message not point")
        
    y = sqrt(F(x**3 + a*x + b))
    return (x,y), ert

def getA(p):
    e = 0
    d = 0
    some = True
    while(some == True):
        e = randint(1,p-1)
        if (GCD(e,p)==1):
            d = inverse(e,p)
            if ((e*d)%p == 1):
                some = False
                break
    return e, d

a = 1768190395030950722223271392002094342061874330567045427440621975506797356658380383
b = 1067148478062573260342098976022767523132549144960198587888542641284046643899146347
p = 1897137590064188545819787018382342682267975428761855001222473056385648716020711551
r = 1505086380229087397070399091939046153619777786308454429781376749
curve = EllipticCurve(GF(p), [a, b])
sock = socket.socket()
file = open("key.txt", "rb")
key = file.read()
file.close()

file = open("test.txt", "rb")
plaintext = file.read()
file.close()

cipher = AES.new(key, AES.MODE_CBC, b'\x00' * AES.block_size)
ciphertext = cipher.encrypt(pad(plaintext, AES.block_size))
file = open("encr.txt", "wb")
file.write(ciphertext)
file.close()
key = key + (b"\x00"*1)
mes_point, qwe = f(key, p,a,b, curve,r)

alpha, invAlpha = getA(curve.cardinality())
P = curve(mes_point[0], mes_point[1])
PAlpha = int(alpha) * P
data_send = encodeClient_p_r_ta(p, r, PAlpha[0], PAlpha[1],a,b)
file = open("first.asn1", "wb")
file.write(data_send)
file.close()
sock.connect(('192.168.116.128', 9090))
sock.send(data_send)
data_recv = sock.recv(1000)
x,y = decodeClient_tab(data_recv)
P = curve(x,y)
PBeta = int(invAlpha) * P
data_send = encodeClient_tb(PBeta, len(ciphertext), ciphertext)
file = open("3.asn1", "wb")
file.write(data_send)
file.close()
sock.send(data_send)

print(key)

#1713066290129542913784217230952328763661172749621461492586786086967878647294
#1713066290129542913784217230952328763661172749621461492586786086967878647294