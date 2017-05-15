#include <iostream>
#include "cryptographer.hpp"
//#include "cryptographer.cpp"
int main() {
    Cryptographer azaza("vl_240_355k_48955581.mp4");

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
    azaza.decrypt("vl_240_355k_48955581.mp4.Sisyph");
    //azaza.encrypt();
    return 0;
}
