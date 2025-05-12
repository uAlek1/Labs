#define _CRT_SECURE_NO_WARNINGS
#include "Crypto.hpp"
#include <bitset>
Crypto::Crypto(std::vector<std::string> data) {
	//data: data[0] = path to file with key
	//      data[1] = sequence tact substitution
	//      data[2] = IV
	try {
		keys = read_key(data.at(0));
		for (char const& c : data.at(1)) {
			tact_substitution.push_back(c);
		}
		//init_counter(data.at(2));
		init_Y0(data.at(2));
	}
	catch (const char* error_message) {
		std::cout << error_message << std::endl;
	}
}
uint32_t Crypto::SP_net(uint32_t block, int i) {
	uint32_t return_block=0;
	block = block ^ keys.at(i).key;
	uint8_t s_block_in;
	std::vector<uint8_t> s_blocks_out;
	for (int i = 0; i < 8; i++) {
		s_block_in = (block << (i*4)) >> 28;
		s_block_in = s_block[s_block_in];
		s_blocks_out.push_back(s_block_in);
	}
	block = 0;
	for (int i = 0; i < 8; i++) {
		block = (block << 4) + s_blocks_out[i];
	}
	s_blocks_out.clear();
	uint32_t transition=0;
	for (int i = 0; i < 32; i++) {
		transition = (A * i + B) % 32;
		if ((block << (31 - i) >> 31) & 1) {
			return_block = return_block ^ (1 << transition);
		}
	}
	return return_block;
}
uint16_t Crypto::SP_net(uint16_t block, int i) {
	uint16_t return_block = 0;
	uint16_t key = keys.at(i).key;
	block = block ^ key;
	uint8_t s_block_in;
	std::vector<uint8_t> s_blocks_out;
	uint16_t qwe = 0;
	for (int i = 0; i < 4; i++) {
		qwe = (block << (i * 4));
		s_block_in = qwe >>12;
		s_block_in = s_block[s_block_in];
		s_blocks_out.push_back(s_block_in);
	}
	block = 0;
	for (int i = 0; i < 4; i++) {
		block = (block << 4) + s_blocks_out[i];
	}
	s_blocks_out.clear();
	uint16_t transition = 0;
	for (int i = 0; i < 16; i++) {
		transition = (A * i + B) % 16;
		if ((block << (15 - i) >> 15) & 1)
			return_block = return_block ^ (1 << transition);
	}

	return return_block;
}
uint32_t Crypto::Feistel(uint32_t block, int i) {
	// (X1||X2) = (X2 || X1 + SP(X2)
	uint32_t return_block;
	uint16_t block1 = block >>16, block2 = block <<16 >>16, result=0;
	return_block = block2;
	return_block <<= 16;
	result = SP_net(block2, i);
	//std::cout << std::hex << std::uppercase << "result SP(block2) = " << result << std::endl; //47FE
	result = result ^ block1;
	return_block += result;
	return return_block;
}
uint32_t Crypto::Ley_Messi(uint32_t block, int i) {
	//X1||X2 = X1 XOR SP(X1 XOR X2) || X2 XOR SP(X1 XOR X2)
	uint32_t return_block;
	uint16_t block1 = block >> 16, block2 = block << 16 >> 16, result = 0;
	uint16_t xor_block = block1 ^ block2;
	result = SP_net(xor_block, i);
	block1 = block1 ^ result;
	return_block = block1;
	return_block <<= 16;
	block2 = block2 ^ result;
	return_block = return_block ^ block2;
	return return_block;

}
void Crypto::encrypt(std::string path, std::string output_path) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in read_blocks";
	}
	uint32_t last_block_size = 0;
	bool flag = 0;
	while (1) {
		uint32_t block = 0;
		unsigned char input_array[4] = { 0, 0, 0, 0 };
		size_t count = fread(input_array, 1, 4, fp);
		if (count == 0) {
			break;
		}

		for (int i = 0; i < count; i++) {
			block <<= 8;
			block += input_array[i];
		}
		if (count < 4) {
			//дополнение неполного блока криптографическим методом
			// работает при количестве блоков >=2
			// Пусть l - длина последнего блока, n- длина блока
			// шифруем предпоследний блок, и берем оттуда n-l правых бит
			// дополняем последний блок этими n-l битами, и шифруем его
			// ставим последний шифрованный блок вместо предпоследнего
			// предпоследний блок ставим последним, при чем не весь блок, а лишь его l левых битов
			if (blocks.size() < 1) {
				break;
			}
			uint32_t last_block = blocks.at(blocks.size() - 1);
			uint32_t count_byte = 4 - count;
			last_block_size = count;
			uint32_t last_block_left = last_block >>(8*count_byte), last_block_right = (last_block <<(8*(4-count_byte))) >>(8*(4-count_byte));
			block <<= (8 * count_byte);
			block += last_block_right;
			block = SP_net(block, 0);
			block = Feistel(block, 1);
			block = SP_net(block, 2);
			block = Ley_Messi(block, 3);
			blocks[blocks.size() - 1] = block;
			last_block_left<<=(8* count_byte);
			blocks.push_back(last_block_left);
			flag = 1;
			break;
		}
		block = SP_net(block,0);
		block = Feistel(block,1);
		block = SP_net(block,2);
		block = Ley_Messi(block,3);
		blocks.push_back(block);
	}

	fclose(fp);
	fp = fopen(output_path.c_str(), "wb");
	if (flag) {
		for (int i = 0; i < blocks.size() - 1; i++) {
			unsigned char write_byte;
			write_byte = blocks[i] >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 8) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 16) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 24) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
		for (int i = 0; i < last_block_size; i++) {
			unsigned char write_byte;
			write_byte = (blocks[blocks.size() - 1] << (8 * i)) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
	}
	else {
		for (int i = 0; i < blocks.size(); i++) {
			unsigned char write_byte;
			write_byte = blocks[i] >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 8) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 16) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 24) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
	}
	fclose(fp);
}



void Crypto::encrypt_CTR(std::string path, std::string output_path) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in read_blocks";
	}
	bool last_block = false;
	int last_block_size = 0;
	while (1) {
		uint32_t block = 0;
		uint32_t counter_32 = 0;
		unsigned char input_array[4] = { 0, 0, 0, 0 };
		size_t count = fread(input_array, 1, 4, fp);
		if (count == 0) {
			break;
		}
		for (int i = 0; i < count; i++) {
			block <<= 8;
			block += input_array[i];
		}
		if (count < 4) {
			last_block = true;
			last_block_size = count;
			counter_32 = counter >> 32;
			counter_32 = encrypt(counter_32);
			counter_32 >>= ((4-count) * 8);
			block = block ^ counter_32;
			block <<= ((4 - count) * 8);
			blocks.push_back(block);
			break;
		}
		counter_32 = counter >> 32;
		counter_32 = encrypt(counter_32);
		block = block ^ counter_32;
		blocks.push_back(block);
		counter = counter * A_CTR + B_CTR;
	}

	fclose(fp);
	fp = fopen(output_path.c_str(), "wb");
	if (last_block) {
		for (int i = 0; i < blocks.size() - 1; i++) {
			unsigned char write_byte;
			write_byte = blocks[i] >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 8) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 16) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 24) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
		for (int i = 0; i < last_block_size; i++) {
			unsigned char write_byte;
			write_byte = (blocks[blocks.size() - 1] << (8 * i)) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
	}
	else {
		for (int i = 0; i < blocks.size(); i++) {
			unsigned char write_byte;
			write_byte = blocks[i] >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 8) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 16) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
			write_byte = (blocks[i] << 24) >> 24;
			fwrite(&write_byte, sizeof(unsigned char), 1, fp);
		}
	}
}

uint32_t Crypto::encrypt(uint32_t block) {
	block = SP_net(block, 0);
	block = Feistel(block, 1);
	block = SP_net(block, 2);
	block = Ley_Messi(block, 3);
	return block;
}

void Crypto::init_counter(std::string path) {
	counter = 0;
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in read_blocks";
	}
	unsigned char input_array[4] = { 0, 0, 0, 0 };
	size_t count = fread(input_array, 1, 4, fp);
	for (int i = 0; i < 4; i++) {
		counter <<= 8;
		counter += input_array[i];
	}
	counter <<= 32;
	//std::cout << std::hex << std::uppercase << counter << std::endl;
}

void Crypto::init_Y0(std::string path) {
	Y0 = 0;
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in init_Y0";
	}
	unsigned char input_array[4] = { 0, 0, 0, 0 };
	size_t count = fread(input_array, 1, 4, fp);
	for (int i = 0; i < 4; i++) {
		Y0 <<= 8;
		Y0 += input_array[i];
	}
}

void Crypto::hash(std::string path, std::string output_path) {
	/*
	*       a b c a1 a2 b1 b2 c1 c2
	* 309 = 1 0 0  1  1  0  1  0  1
	* 1 - доп блок помещать вначало
	* 0 - неполный блок дополнять справа битами со значением 0
	* 0 - дополнительный блок содержит длину сообщения в байтах
	* F_{1*x + 1y}(0*x + 1*y) + 0*x + 1*y
	* 
	Пояснения к содержимому.
Биты 9-разрядного числа A = abcdefghi кодируют следующие параметры:
a: 0 - дополнительный блок помещать в конец, 1 - помещать в начало 
b: неполный блок дополнять справа битами со значением b
с: 0 - дополнительный блок содержит длину сообщения в байтах, 1 - содержит длину последнего блока в байтах
defghi: определяют, соответственно, коэффициенты a1, a2, b1, b2, c1 и с2 ш.ф.х. F_{a1*X + a2*Y}(b1*X + b2*Y) + c1*X + c2*Y. 
	*/
	read_words(path);
	//std::cout << "start\n";
	//for (int i = 0; i < blocks.size(); i++) {
	//	std::cout << std::hex << std::uppercase << blocks[i] << std::endl;
	//}
	//std::cout << "end\n";
	std::cout << std::dec << len_message << std::endl;
	uint32_t Y = Y0;
	std::cout << "Y0 = " << std::hex << std::uppercase << Y0 << std::endl;
	for (int i = 0; i < blocks.size(); i++) {
		/*
		Yi = g(Xi, Y(i-1))
		g(X,Y) = F_{X + Y}(Y) + Y 
		*/
		/*std::cout << "block\n";
		std::cout << std::hex << std::uppercase << blocks[i] << std::endl;
		std::cout << std::hex << std::uppercase << Y << std::endl;
		std::cout << "end block\n";*/
		Y = g(blocks[i], Y);
		/*std::cout << "new block\n";
		std::cout << std::hex << std::uppercase << Y << std::endl;
		std::cout << "\n";*/
	}
	std::cout << "Y = " <<std::hex<<std::uppercase << Y << std::endl;
	FILE* fp = fopen(output_path.c_str(), "wb");
	if (fp == NULL) {
		throw "ERROR open file in read_blocks";
	}
	unsigned char write_byte=0;
	write_byte = Y >> 24;
	fwrite(&write_byte, sizeof(unsigned char), 1, fp);
	write_byte = (Y << 8) >> 24;
	fwrite(&write_byte, sizeof(unsigned char), 1, fp);
	write_byte = (Y << 16) >> 24;
	fwrite(&write_byte, sizeof(unsigned char), 1, fp);
	write_byte = (Y << 24) >> 24;
	fwrite(&write_byte, sizeof(unsigned char), 1, fp);
	fclose(fp);
}
uint32_t Crypto::g(uint32_t X, uint32_t Y) {
	get_key(X ^ Y);
	/*std::cout << '\n';
	for (int i = 0; i < keys.size(); i++) {
		std::cout << std::hex << std::uppercase << keys[i].key << std::endl;
	}
	std::cout << '\n';*/
	return encrypt(Y) ^ Y;
}
void Crypto::get_key(uint32_t key) {
	keys.clear();
	uint16_t key1 = 0, key2 = 0;
	key1 = key >> 16;
	key2 = key & 0x0000FFFF;
	/*Расписание ключей
	  k1<15,k1 (k1^15634)|(k2) k1,k2^22845 k1^27398 */
	tact_key tmp;
	uint16_t temp_key = 0;
	temp_key = (key1 << 15) | (key1 >> (16 - 15));
	tmp.key = temp_key;
	tmp.key <<= 16;
	tmp.key += key1;
	keys.push_back(tmp);

	temp_key = (key1 ^ 15634) | key2;
	tmp.key = temp_key;
	keys.push_back(tmp);

	tmp.key = key1;
	tmp.key <<= 16;
	temp_key = key2 ^ 22845;
	tmp.key += temp_key;
	keys.push_back(tmp);

	temp_key = key1 ^ 27398;
	tmp.key = temp_key;
	keys.push_back(tmp);
}
void Crypto::read_words(std::string path) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in read_blocks";
	}
	while (1) {
		uint32_t block = 0;
		unsigned char input_array[4] = { 0, 0, 0, 0 };
		size_t count = fread(input_array, 1, 4, fp);
		if (count == 0) {
			break;
		}
		for (int i = 0; i < count; i++) {
			block <<= 8;
			block += input_array[i];
		}
		if (count < 4) {
			len_message += count;
			block <<= (8 * (4 - count));
			blocks.push_back(block);
			blocks.insert(blocks.begin(), len_message);
			break;
		}
		len_message += 4;
		blocks.push_back(block);
	}
	fclose(fp);
}

std::vector<tact_key> Crypto::read_key(std::string path) {
	std::vector<tact_key> return_vec;
	uint32_t key=0;
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL) {
		throw "ERROR open file in read_key";
	}
	unsigned char s;
	s = fgetc(fp);
	key = key + s;
	for (int i = 0; i < 3; i++) {
		key <<= 8;
		s = fgetc(fp);
		key = key + s;
	}
	fclose(fp);
	uint16_t key1 = 0, key2 = 0;
	key1 = key >> 16;
	key2 = key & 0x0000FFFF;
	//std::cout << std::hex << std::uppercase << key1 << std::endl;
	//std::cout << std::hex << std::uppercase << key2 << std::endl;
	/*Расписание ключей
	  k1<15,k1 (k1^15634)|(k2) k1,k2^22845 k1^27398 */
	tact_key tmp;
	uint16_t temp_key=0;
	temp_key  = (key1 << 15) | (key1 >> (16 - 15));
	tmp.key = temp_key;
	tmp.key <<= 16;
	tmp.key += key1;
	return_vec.push_back(tmp);

	temp_key = (key1 ^ 15634) | key2;
	tmp.key = temp_key;
	return_vec.push_back(tmp);

	tmp.key = key1;
	tmp.key <<= 16;
	temp_key = key2 ^ 22845;
	tmp.key += temp_key;
	return_vec.push_back(tmp);

	temp_key = key1 ^ 27398;
	tmp.key = temp_key;
	return_vec.push_back(tmp);
	return return_vec;

}
Crypto::~Crypto() {
	keys.clear();
	blocks.clear();
	tact_substitution.clear();
}




/*
* 
* 
* key = 0C 65 05 D5
* IV  = 29 5E 54 26
* counter init 29 5E 54 26 00 00 00 00
* msb(counter) = 29 5E 54 26
* 694047782 ==   29 5E 54 26
* 3629005860 ==  D8 4E 3C 24
* encrypt(29 5E 54 26) ---верю, что у меня шифрует правильно
* тогда 
* encrypt(29 5E 54 26) = D8 4E 3C 24
* block1 = block1 XOR encrypt(29 5E 54 26)
* block1 = 00 00 3E 86 ^ D8 4E 3C 24 = D8 4E 02 A2
* new counter =  716205195 *cointer_init  + 1193358046 = 57D7 B8A2 4721 32DE
* msb() = 57D7 B8A2
* 1 473 755 298
*   595 210 473 == 237A 30E9
* block2 = block2 XOR 237A 30E9 = 00 00 7E C0 XOR 23 7A 30 E9  = 23 7A 4E 29

00 00 3E 86








00 00 7E C0

*/