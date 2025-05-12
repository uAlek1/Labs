#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Crypto.hpp"

int main(int argc, char** argv) {
	std::vector<std::string> data;
	std::string path_to_key;
	std::string path_to_test;
	std::string path_to_output;
	std::string path_IV;
	for (int i = 1; i < 11; i++) {

		path_to_key = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\key" + std::to_string(i) + ".in";
		path_to_test = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\test" + std::to_string(i) + ".in";
		path_to_output = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\test" + std::to_string(i) + ".out";
		path_IV = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\H0_" + std::to_string(i) + ".in";
		std::cout << path_to_key << '\n' << path_to_test << '\n' << path_to_output << '\n'<< path_IV << std::endl;
		data.push_back(path_to_key);
		data.push_back("sfsl");
		data.push_back(path_IV);
		Crypto test(data);
		test.hash(path_to_test, path_to_output);
		data.clear();
	}
	/*path_to_key = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\example_key.in";
	path_to_test = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\example2.in";
	path_to_output = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\example100.out";
	path_IV = "C:\\Users\\alust\\source\\repos\\KMZI1\\KMZI1\\H0_1.in";
	data.push_back(path_to_key);
	data.push_back("sfsl");
	data.push_back(path_IV);
	Crypto test(data);
	test.hash(path_to_test);
	data.clear();*/
}