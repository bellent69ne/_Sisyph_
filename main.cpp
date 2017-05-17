#include <iostream>
#include "cryptographer.hpp"
#include "walker.hpp"
//#include "secblock.h"

int main() {
    Cryptographer azaza;

    //Cryptographer za(azaza);
//    std::wcout << azaza.currentPath() << std::endl;
//    azaza.encrypt();

    //azaza.encrypt("~/Videos");
    //std::wcout << azaza.currentPath() << std::endl;
    azaza.generateKey();
    std::cout << "Key: " << azaza.getKey() << std::endl;
    azaza.generateIV();
    std::cout << "IV: " << azaza.getIV() << std::endl;

    //azaza.encrypt();
    azaza.setKey("62C8E4337FE499299414123E17266F35");
    azaza.setIV("3AB0D4564A51E29674E815493C14D115");
   // azaza.decrypt("vl_240_355k_48955581.mp4");
   // azaza.encrypt();

    Walker shadowWalker("/home/maezers/projects/_Sisyph_/azaza/", &azaza);
    shadowWalker.walk();

    return 0;
}
