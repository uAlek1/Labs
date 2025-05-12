import asn1

def encodeClient_p_r_ta(p, r, t_a_x, t_a_y, a, b):
    asn1_encoder = asn1.Encoder()
    asn1_encoder.start()
    # Sequence HEADER
    asn1_encoder.enter(asn1.Numbers.Sequence)
    #Key set
    asn1_encoder.enter(asn1.Numbers.Set)
    # Sequence_2 start
    asn1_encoder.enter(asn1.Numbers.Sequence)
    # 0x80070200 - Месси-Омуры
    asn1_encoder.write(b'\x80\x07\x02\x00', asn1.Numbers.OctetString)
    asn1_encoder.write(b'mo', asn1.Numbers.UTF8String)
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    #
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(int(p), asn1.Numbers.Integer)
    asn1_encoder.write(int(r), asn1.Numbers.Integer)
    asn1_encoder.write(int(a), asn1.Numbers.Integer)
    asn1_encoder.write(int(b), asn1.Numbers.Integer)
    asn1_encoder.leave()
    asn1_encoder.leave()
    #
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(int(t_a_x), asn1.Numbers.Integer)
    asn1_encoder.write(int(t_a_y), asn1.Numbers.Integer)
    asn1_encoder.leave()
    # Sequence_2 end
    asn1_encoder.leave()

    # Set_1 end
    asn1_encoder.leave()

    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()

    return asn1_encoder.output()

def decodeServer_p_r_ta(data):

    #data = file.read()
    decoder = asn1.Decoder()
    decoder.start(data)
    
    decoder.enter() #-sequence header
    decoder.enter() #set
    decoder.enter() #sequence key

    decoder.read() # идентификатор алгоритма (протокол Месси–Омуры) 80 07 02 00
    decoder.read() # может не задействоваться или использоваться для идентификации будущего переданного сообщения

    decoder.enter()
    decoder.leave()

    decoder.enter()
    p = decoder.read()[1]
    r = decoder.read()[1]
    a = decoder.read()[1]
    b = decoder.read()[1]
    decoder.leave()

    decoder.leave()

    decoder.enter()
    t_a_x = decoder.read()[1]
    t_a_y = decoder.read()[1]
    decoder.leave()

    decoder.leave()
    decoder.leave()

    decoder.enter()
  
    decoder.leave()  

    return p, r, t_a_x, t_a_y, a, b #decoded_parameters[0], decoded_parameters[1], decoded_parameters[2]

def encodeServer_t_ab(t_ab):
    asn1_encoder = asn1.Encoder()
    asn1_encoder.start()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.enter(asn1.Numbers.Set)
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(b'\x80\x07\x02\x00', asn1.Numbers.OctetString)
    asn1_encoder.write(b'mo', asn1.Numbers.UTF8String)
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(int(t_ab[0]), asn1.Numbers.Integer)
    asn1_encoder.write(int(t_ab[1]), asn1.Numbers.Integer)
    asn1_encoder.leave()
    asn1_encoder.leave()
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    asn1_encoder.leave()
    return asn1_encoder.output()

def decodeClient_tab(data):
    decoder = asn1.Decoder()
    decoder.start(data)
    decoder.enter() 
    decoder.enter()
    decoder.enter()
    decoder.read() 
    decoder.read() 
    decoder.enter()
    decoder.leave()
    decoder.enter()
    decoder.leave()
    decoder.enter()
    x = decoder.read()[1]
    y = decoder.read()[1]
    decoder.leave()
    decoder.leave()
    decoder.leave()
    decoder.enter()
    decoder.leave()  
    decoder.leave()  
    return x,y

def encodeClient_tb(t_b,len, cyphertext):
    asn1_encoder = asn1.Encoder()
    asn1_encoder.start()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.enter(asn1.Numbers.Set)
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(b'\x80\x07\x02\x00', asn1.Numbers.OctetString)
    asn1_encoder.write(b'mo', asn1.Numbers.UTF8String)
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(int(t_b[0]), asn1.Numbers.Integer)
    asn1_encoder.write(int(t_b[1]), asn1.Numbers.Integer)
    asn1_encoder.leave()
    asn1_encoder.leave()
    asn1_encoder.leave()
    asn1_encoder.enter(asn1.Numbers.Sequence)
    asn1_encoder.write(b'\x10\x82', asn1.Numbers.OctetString)
    asn1_encoder.write(len, asn1.Numbers.Integer)
    asn1_encoder.write(cyphertext)
    asn1_encoder.leave()
    asn1_encoder.leave()
    return asn1_encoder.output()

def decodeServer_tb(data):
    decoder = asn1.Decoder()
    decoder.start(data)
    decoder.enter() 
    decoder.enter()
    decoder.enter() 
    decoder.read() 
    decoder.read() 
    decoder.enter()
    decoder.leave()
    decoder.enter()
    decoder.leave()
    decoder.enter()
    x = decoder.read()[1]
    y = decoder.read()[1]
    decoder.leave()
    decoder.leave()
    decoder.leave()
    decoder.enter()
    decoder.read()
    len = decoder.read()[1]
    ciphertext = decoder.read()[1]
    decoder.leave()  
    decoder.leave()  
    return x, y, len, ciphertext