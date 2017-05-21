#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
#include "inpArgsCorrector.hpp"
#include <fstream>

int main(int argc, char **argv) {
	//InpArgsCorrector correctInput(argc, argv);

    auto coolCryptor(std::make_shared<Cryptographer>());
    //std::cout << correctInput.actualKey() << std::endl;
    //std::cout << correctInput.actualPath() << std::endl;
    //if(correctInput.actualKey() == "--keyGen") {
     //   coolCryptor->generateKey();
     //   coolCryptor->generateIV();
   // }
    //else
      //  coolCryptor->setKey(correctInput.actualKey());
    
    //auto saveKey(static_cast<std::ofstream>("confidential.dat"));
    //saveKey << coolCryptor->getKey() + coolCryptor->getIV();
    //saveKey.close();

    Walker shadowWalker("/home", coolCryptor);
   // coolCryptor->willEncrypt(correctInput.wannaEncrypt());
    //shadowWalker.walk(correctInput.isRecursive());

    shadowWalker.setCmdArgs(argc, argv);
    shadowWalker.walk();
//    std::cout << "KEY.... DON'T LOSE IT >>>>>>>>>>>>>>> " 
  //            << coolCryptor->getKey() + coolCryptor->getIV() << std::endl;
     

    return 0;
}
