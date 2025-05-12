#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <iostream>
#include <vector>
#include <fstream>

typedef struct tact_key {
	uint32_t key;
}tack_key;

class Crypto {
public:
	Crypto(std::vector<std::string>);
	~Crypto();
	void encrypt(std::string, std::string);
	void encrypt_CTR(std::string, std::string);
	void hash(std::string, std::string);
private:
	uint32_t SP_net(uint32_t, int);
	uint16_t SP_net(uint16_t, int);
	uint32_t Feistel(uint32_t, int);
	uint32_t Ley_Messi(uint32_t, int);
	uint32_t Tau_substitution(uint32_t, int);
	uint32_t encrypt(uint32_t);
	void read_words(std::string);
	std::vector<tact_key> read_key(std::string);
	uint32_t g(uint32_t, uint32_t);
	void get_key(uint32_t);
	std::vector<uint32_t> blocks;
	std::vector<char> tact_substitution;
	std::vector<tact_key> keys;
	std::vector<int> s_block{ 11, 7, 14, 3, 9, 15, 0, 6, 13, 1, 10, 12, 8, 2, 4, 5 };
	int A = 21, B = 23;
	uint32_t A_CTR = 716205195, B_CTR = 1193358046;
	uint64_t counter;
	uint32_t Y0;
	void init_counter(std::string);
	void init_Y0(std::string);
	uint32_t len_message;
};


#endif