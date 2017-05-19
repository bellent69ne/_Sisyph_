#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
#include "inpArgsCorrector.hpp"
#include <fstream>

int main(int argc, char **argv) {
	InpArgsCorrector correctInput(argc, argv);

    auto coolCryptor(std::make_shared<Cryptographer>());
    if(correctInput.actualKey() == "--keyGen") {
        coolCryptor->generateKey();
        coolCryptor->generateIV();
    }
    else
        coolCryptor->setKey(correctInput.actualKey());
    
    auto saveKey(static_cast<std::ofstream>("confidential.dat"));
    saveKey << coolCryptor->getKey() + coolCryptor->getIV();
    saveKey.close();

    Walker shadowWalker(correctInput.actualPath(), coolCryptor);
    coolCryptor->willEncrypt(correctInput.wannaEncrypt());
    shadowWalker.walk(correctInput.isRecursive());

    std::cout << "KEY.... DON'T LOSE IT >>>>>>>>>>>>>>> " 
              << coolCryptor->getKey() + coolCryptor->getIV() << std::endl;
     

    return 0;
}
