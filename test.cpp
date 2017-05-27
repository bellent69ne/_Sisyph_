#include "blockCipher.hpp"
#include "cbctwofish.hpp"
#include <iostream>

int main(int argc, char **argv) {
	BlockCipher azaza = std::make_shared<CBCTwofish>(argv[1]);

	return 0;
}