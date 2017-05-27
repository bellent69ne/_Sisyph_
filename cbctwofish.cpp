#include "cbctwofish.hpp"
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>
#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include "twofish.h"
using CryptoPP::Twofish;

#include "modes.h"
using CryptoPP::CBC_Mode;

#include "secblock.h"
using CryptoPP::SecByteBlock;


/**** Implementation of Encryption method itself *****/
/**** WRONG!!!!!! ********************************
******** SMTH BAD HAPPENS HERE************************/
CBCTwofish::CBCTwofish(): m_currentPath(""),
                                //m_keyLength(Twofish::MAX_KEYLENGTH),
                                m_encKey(""),
                                m_encIV(""),
                                m_byteKey(Twofish::MAX_KEYLENGTH),
                                m_willEncrypt(true) {
}



void CBCTwofish::encrypt() {
    if(m_currentPath.extension() == ".sisyph") {
        std::cerr << "File is already encrypted..." << std::endl;
        exit(1);
    }

    /***********************************************************/

    process<CBC_Mode<Twofish>::Encryption>(".sisyph");      
}

/*****************************************************/

void CBCTwofish::decrypt() {
    if(m_currentPath.extension() != ".sisyph") {
        std::cerr << "The file is decrypted..." << std::endl;
        exit(1);
    }
    
    /********************************************************/

    process<CBC_Mode<Twofish>::Decryption>("");
}

void CBCTwofish::generateKey() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteKey, m_byteKey.size());

    m_encKey.clear();
    StringSource ssKey(m_byteKey, m_byteKey.size(), true,
        new HexEncoder (
            new StringSink(m_encKey)
        ) // For HexEncoder
    );  // For StringSource
    /*From this point we have our key in m_encKey(probably
                                                  encoded)*/;
}

void CBCTwofish::generateIV() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteIV, sizeof(m_byteIV));

    m_encIV.clear();
    StringSource ssIV(m_byteIV, sizeof(m_byteIV), true,
        new HexEncoder (
            new StringSink(m_encIV)
        )   // For HexEncoder
    );  // For StringSource
}
