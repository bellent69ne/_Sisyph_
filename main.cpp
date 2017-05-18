#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
//#include "secblock.h"
#include "corrector.hpp"
#include <fstream>

int main(int argc, char **argv) {
	Corrector correctInput(argc, argv);
	//std::cout << correctInput.actualKey();
	auto azaza(std::make_shared<Cryptographer>());

	if(correctInput.actualKey().length() <= 3)
		azaza->generateKey(std::abs(std::stoi(correctInput.actualKey())));
	else
    	azaza->setKey(correctInput.actualKey());		//That's a pain
    azaza->setIV("3AB0D4564A51E29674E815493C14D115");
   // azaza.decrypt("vl_240_355k_48955581.mp4");
   // azaza.encrypt();

    Walker shadowWalker(correctInput.path(), azaza);
    azaza->willEncrypt(correctInput.wannaEncrypt());
    shadowWalker.walk(true);

    if(azaza->goingToEncrypt()) {
        std::cout << "Key...DON'T LOSE IT!!------->  " <<azaza->getKey() << std::endl;

    //    std::ofstream keyStream("/home/maezers/projects/_Sisyph_/dontLoseIt.dat");
  //      keyStream << azaza->getKey();

//        keyStream.close();
    }    

    return 0;
}
