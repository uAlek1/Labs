from Crypto.Cipher import AES
from Crypto import Random
from Crypto.Util.Padding import pad, unpad
from Crypto.Util.number import bytes_to_long, long_to_bytes, getPrime
from math import gcd
from hashlib import sha256
import asn1
import sys
import os



class AES_Crypto:
    def __init__(self):
        pass
    def gen_key(self) -> bytes:
        return Random.get_random_bytes(32)

    def encrypt(self, plaintext:bytes, key: bytes) -> bytes:
        cipher = AES.new(key, AES.MODE_CBC, b'\x00' * AES.block_size)
        ciphertext = cipher.encrypt(pad(plaintext, AES.block_size))
        encrypted_data = ciphertext
        return encrypted_data

    def decrypt(self, ciphertext: bytes, key: bytes) -> bytes:
        iv = b'\x00' * AES.block_size
        cipher = AES.new(key, AES.MODE_CBC, iv)
        decrypted_data = unpad(cipher.decrypt(ciphertext), AES.block_size)
        return decrypted_data

class RSA:
    def gen_key(self, bits):
        e = 65537
        while True:
            p, q = getPrime(bits), getPrime(bits)
            n, phi = p * q, (p - 1) * (q - 1)
            if gcd(phi, e) == 1:
                d = self._bezout(e, phi)
                if(d < 0):
                    d = phi + d
                break

        return e,d,n

    def encrypt(self, plaintext: bytes, e: int, n:int) -> bytes:
        num = bytes_to_long(plaintext)
        num_enc = pow(num, e, n)
        ciphertext = long_to_bytes(num_enc)
        return ciphertext

    def decrypt(self, ciphertext: bytes, d:int, n:int) -> bytes:
        num_enc = bytes_to_long(ciphertext)
        num = pow(num_enc, d, n)
        plaintext = long_to_bytes(num)
        return plaintext

    def signature(self, message: bytes, d:int, n:int) -> int:
        hash = int.from_bytes(sha256(message).digest(), byteorder='big')
        signature = pow(hash, d, n)
        return signature

    def verify(self, message: bytes, signature: int, e:int, n:int) -> bool:
        hash = int.from_bytes(sha256(message).digest(), byteorder='big')
        hashFromSignature = pow(signature, e, n)
        return hash == hashFromSignature

    def _bezout(self, a, b):
        x, xx, y, yy = 1, 0, 0, 1
        while b:
            q = a // b
            a, b = b, a % b
            x, xx = xx, x - xx * q
            y, yy = yy, y - yy * q
        return x

class ASN1_RSA:
    def asn_encoder(self, data, e, n, lenth, encrypted, code):
        encoder = asn1.Encoder()
        encoder.start()
        encoder.enter(asn1.Numbers.Sequence)
        encoder.enter(asn1.Numbers.Set)
        encoder.enter(asn1.Numbers.Sequence)
        if code == 'encrypt':
            encoder.write(b'\x00\x01', asn1.Numbers.OctetString)
        elif code == 'signature':
            encoder.write(b'\x00\x40', asn1.Numbers.OctetString)
        else: 
            print ('error')
            exit(-1)
        encoder.write(b'\x0C\x00', asn1.Numbers.UTF8String)
        encoder.enter(asn1.Numbers.Sequence)  
        encoder.write(n, asn1.Numbers.Integer)
        encoder.write(e, asn1.Numbers.Integer)
        encoder.leave() 
        encoder.enter(asn1.Numbers.Sequence)
        encoder.write(data, asn1.Numbers.Integer)
        encoder.leave()
        encoder.leave()
        encoder.leave() 
        
        if code == 'encrypt':
            encoder.enter(asn1.Numbers.Sequence)
            encoder.write(b'\x10\x82', asn1.Numbers.OctetString)
            encoder.write(lenth, asn1.Numbers.Integer)    
            encoder.write(encrypted, asn1.Numbers.OctetString)
            encoder.leave()
        
        encoder.leave()  
        
        return encoder.output()
    
    def asn_decoder(self, decoder, parameters):
        while not decoder.eof():
            try:
                tag = decoder.peek()
                if tag.nr == asn1.Numbers.Null:
                    break
                if tag.typ == asn1.Types.Primitive:
                    tag, value = decoder.read()
                    # Если тип Integer
                    if tag.nr == asn1.Numbers.Integer: 
                        # Добавляем значение в массив
                        parameters.append(value)
                else:
                    decoder.enter()
                    self.asn_decoder(decoder, parameters)
                    decoder.leave()

            except asn1.Error:
                break

e = 0x10001
d = 0x7dbe9a86cc9673e09d37f470c1014c5db1e7a14b9422c6ed5fe5770d4f6209e646205a11d2bf7bd82e218b59630f2690536ac1abd33f2162d44684d414e080ba626de34e562c32d866e3fd2d438218fdcc617d9ca633e3f35905cd4dad3be48ee7c740eb480bfa8d1140938f466f95713aa03432d0243b1097c55cb87df0d80d6fff5e7b14be4a9c4c59bb465f872db81ddafb1371155883e7119075a239d33a7b0319b63d391218bc016daef9527430b14be61f04f5beba2711ac63fcc02e12b42bcd0180aa477bff6b8673a7be307452317e4c903c1fe22b0193c968c52349d9d1db4a92f2e170ed849fc053b84314e998359311e52649e0cfd8a487baa701
n = 0x80a0b1c0b4264c134fd663089ac9a9c898902dfae00b1abd7d6e1a51b2ed62895bb067c53bd4648826b7bf271276689f6c2b5dc4466ed842107ff6e66b02364a34ae5d9d09ffe04eddfd5cf23dc9acfbd992f099d5f7817183e995ca6f603dac7546a9e8dc46664ba59733dd42c9d95b3721935c081565e93834401d83e9c68e4cb6e7a353a4a302dcb6a4a1e6240690ef5e4f9d16926dcf5112752dd93f8a8989d873098cb9ec8768f26c91fc68f8c1f9f0413cb2488c3e434e0edc108010010eb6670e9ed4dd83a447e88df788aa8ee91a8f5b64bef250d5a3687c46f93d41066fa9e7adeeb3ad73f8045cc929d48361e2c0396b446ab283ae062e9e328875
 
def main():
    aes = AES_Crypto()
    rsa = RSA()
    asn1file = ASN1_RSA()

    if len(sys.argv) == 3 and sys.argv[2] == "-enc":
        print('File encryption...\n')
        aesKey = aes.gen_key()
        with open(sys.argv[1], "rb") as file:
            data = file.read()
            file.close()
        encData = aes.encrypt(data, aesKey)
        encAesKey = rsa.encrypt(aesKey, e, n)
        asn1Text = asn1file.asn_encoder(bytes_to_long(encAesKey), e, n, len(encData), encData, 'encrypt')
        with open(sys.argv[1]+'.asn1', "wb") as file:
            file.write(asn1Text)
            file.close()
        print("file encrypted")
        print(f"result in {sys.argv[1]}.asn1")

    elif len(sys.argv) == 3 and sys.argv[2] == "-dec":
        print(u'\nFile decryption...\n')
        parameters = []
        with open(sys.argv[1], "rb") as file:
            text = file.read()
            file.close()
        decoder = asn1.Decoder()
        decoder.start(text)
        asn1file.asn_decoder(decoder, parameters)
        encData = text[-parameters[-1]:]
        public_n = parameters[0]
        public_e = parameters[1]
        encAesKey = parameters[2]
        aesKey = rsa.decrypt(long_to_bytes(encAesKey), d, n)
        data = aes.decrypt(encData, aesKey)
        path = os.path.splitext(sys.argv[1])
        with open(path[0], "wb") as file:
            file.write(data)
            file.close()
        print("file decrypted")
        print(f"result in {path[0]}")

               
    elif len(sys.argv) == 3 and sys.argv[2] == "-gen":
        print('signature generation...\n')
        with open(sys.argv[1], "rb") as file:
            data = file.read()
            file.close()
        
        sign = rsa.signature(data, d, n)
        asn1_text = asn1file.asn_encoder(sign, d, n, 0, 0, 'signature')
        with open(sys.argv[1]+".asn1", "wb") as file:
            file.write(asn1_text)
            file.close()
        print("file sign")
        print(f"result in {sys.argv[1]}.asn1")
        
    elif len(sys.argv) == 3 and sys.argv[2] == "-ver":
        print('signature verification...\n')
        with open(sys.argv[1], "rb") as file:
            data = file.read()
            file.close()
        path = os.path.splitext(sys.argv[1])
        parameters = []
        decoder = asn1.Decoder()
        decoder.start(data)
        asn1file.asn_decoder(decoder, parameters)
        public_n = parameters[0]
        sign = parameters[2]
        datafile = str(input("enter datafile: "))
        with open(datafile, "rb") as file:
            data = file.read()
            file.close()
        if(rsa.verify(data,sign,e,public_n)):
            print("correct")
        else:
            print("not correct")

        
    else:
        print("incorrect usage")

if __name__ == '__main__':
    main()
