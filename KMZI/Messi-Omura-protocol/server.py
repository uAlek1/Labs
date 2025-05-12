import socket
from sage.all import *
from asn1mo import *
from random import randint
from Crypto.Util.number import long_to_bytes, inverse
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad

def f(P)->bytes:
    x = int(P[0])
    x = long_to_bytes(x)
    return x[:-1]

def getB(p):
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

sock = socket.socket()
sock.bind(('192.168.116.128', 9090))
sock.listen(1)
conn, addr = sock.accept()
print("Connect")
while True:
    data = conn.recv(1000000)
    if not data:
        break
    p,r,x,y,a,b = decodeServer_p_r_ta(data)
    curve = EllipticCurve(GF(p), [a, b])
    beta, invBeta = getB(curve.cardinality())
    P = curve(x,y)
    PAB = int(beta) * P
    data_send = encodeServer_t_ab(PAB)
    file = open("2.asn1", "wb")
    file.write(data_send)
    file.close()
    conn.send(data_send)
    data_2 = conn.recv(100000000)
    x,y,length,ciphertext = decodeServer_tb(data_2)
    P = curve(x,y)
    P_real = int(invBeta) * P
    print(P_real)
    key = f(P_real)
    print(key)

    iv = b'\x00' * AES.block_size
    cipher = AES.new(key, AES.MODE_CBC, iv)
    decrypted_data = unpad(cipher.decrypt(ciphertext), AES.block_size)

    file = open("testserver.txt", "wb")
    file.write(decrypted_data)
    file.close()

conn.close()
