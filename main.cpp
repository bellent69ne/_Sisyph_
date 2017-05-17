#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
//#include "secblock.h"

int main() {
	auto azaza(std::make_shared<Cryptographer>());

    azaza->setKey("62C8E4337FE499299414123E17266F35");		//That's a pain
    azaza->setIV("3AB0D4564A51E29674E815493C14D115");
   // azaza.decrypt("vl_240_355k_48955581.mp4");
   // azaza.encrypt();

    Walker shadowWalker("/home/maezers/projects/_Sisyph_/azaza/", azaza);
    azaza->willEncrypt(false);
    shadowWalker.walk(true);

    return 0;
}