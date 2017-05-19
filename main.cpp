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
    
    Walker shadowWalker(correctInput.actualPath(), coolCryptor);
    coolCryptor->willEncrypt(correctInput.wannaEncrypt());
    shadowWalker.walk(correctInput.isRecursive());

    std::cout << "KEY.... DON'T LOSE IT >>>>>>>>>>>>>>> " 
              << coolCryptor->getKey() + coolCryptor->getIV() << std::endl;
    //std::cout << "path >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << correctInput.actualPath() << std::endl;
	//std::cout << correctInput.actualKey();
	//auto azaza(std::make_shared<Cryptographer>());

	//if(correctInput.actualKey().length() <= 3)
	//	azaza->generateKey(std::abs(std::stoi(correctInput.actualKey())));
	//else
    	//azaza->setKey(argv[1]);		//That's a pain
    //azaza->setIV("3AB0D4564A51E29674E815493C14D115");
    //azaza->generateKey();
    //azaza->generateIV();
    //azaza->setKey("7C7CB07B11DB2BB951D80882E80FE45484C621A96EE7C86E25A25BD99E6D9152");
    //azaza->setIV("614469D0A91428FBB522D5D3128F1260");
    //Walker shadowWalker(argv[1], azaza);
    //azaza->willEncrypt(false);
    //shadowWalker.walk(true);

    //if(azaza->goingToEncrypt()) {
      //  std::cout << "Key...DON'T LOSE IT!!------->  " 
        //          << azaza->getKey() + azaza->getIV() << std::endl;
        //std::cout << "IV...----------------------->  " << azaza->getIV() << std::endl;
    //    std::ofstream keyStream("/home/maezers/projects/_Sisyph_/dontLoseIt.dat");
  //      keyStream << azaza->getKey();

//        keyStream.close();
    //}    

    return 0;
}
