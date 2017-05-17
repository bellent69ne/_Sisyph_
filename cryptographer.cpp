#include "cryptographer.hpp"
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
Cryptographer::Cryptographer(): m_currentPath(""),
                                m_keyLength(0),
                                m_encKey(""),
                                m_encIV(""), 
                                m_goingToEncrypt(true) {
}



void Cryptographer::encrypt() const {
    if(m_currentPath.extension() == ".Sisyph") {
        std::cerr << "File is already encrypted..." << std::endl;
        exit(1);
    }

    /***********************************************************/

    try {
        CBC_Mode<Twofish>::Encryption cbcTwofish;
        cbcTwofish.SetKeyWithIV(*m_byteKey, m_byteKey->size(),
                                m_byteIV);
        FileSource file(m_currentPath.generic_string().c_str(), true,
            new StreamTransformationFilter(cbcTwofish,
                new FileSink((m_currentPath.generic_string() 
                              + ".Sisyph").c_str()
                )
            )
        );

    } catch(const CryptoPP::Exception& e) {
          std::cerr << e.what() << std::endl;
          exit(1);
      }
          /*auto cbc(std::make_unique<CBC_Mode<Twofish>::Encryption>());
    process(cbc, true);*/
    /*******We should provide our implementation to system's shred*******/
    std::system(("shred -u " + m_currentPath.generic_string()).c_str());
}

/*****************************************************/

void Cryptographer::decrypt() const {
    if(m_currentPath.extension() != ".Sisyph") {
        std::cerr << "The file is decrypted..." << std::endl;
        exit(1);
    }
    
    /********************************************************/

    try {
        CBC_Mode<Twofish>::Decryption cbcTwofishD;
        cbcTwofishD.SetKeyWithIV(*m_byteKey, m_byteKey->size(),
                                 m_byteIV);
  
        auto sourceFile(m_currentPath);
        sourceFile.replace_extension("");

        FileSource file(m_currentPath.generic_string().c_str(), true,
            new StreamTransformationFilter(cbcTwofishD,
                new FileSink(sourceFile.generic_string().c_str()
                )
            )
        );
    } catch(const CryptoPP::Exception &e) {
          std::cerr << e.what() << std::endl;
          exit(1);
      }
    //process(std::make_unique<CBC_Mode<Twofish::Decryption>>(), false);
    /*We should implement "shred" ourself*/
    system(("shred -u " + m_currentPath.generic_string()).c_str());
}

void Cryptographer::generateKey() {
    auto isBadKey([m_keyLength = m_keyLength] {
        return (m_keyLength <= 0) && 
               (m_keyLength != Twofish::DEFAULT_KEYLENGTH) ||
               (m_keyLength != Twofish::MAX_KEYLENGTH);
        });

    if(isBadKey())
        m_keyLength = Twofish::DEFAULT_KEYLENGTH;

    //SecByteBlock key(m_keyLength);
    m_byteKey = std::make_unique<SecByteBlock>(m_keyLength);
    AutoSeededRandomPool prng;
    prng.GenerateBlock(*m_byteKey, m_byteKey->size());

    m_encKey.clear();
    StringSource ssKey(*m_byteKey, m_byteKey->size(), true,
        new HexEncoder (
            new StringSink(m_encKey)
        ) // For HexEncoder
    );  // For StringSource
    /*From this point we have our key in m_encKey(probably
                                                  encoded)*/;
}

void Cryptographer::generateIV() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteIV, sizeof(m_byteIV));

    m_encIV.clear();
    StringSource ssIV(m_byteIV, sizeof(m_byteIV), true,
        new HexEncoder (
            new StringSink(m_encIV)
        )   // For HexEncoder
    );  // For StringSource
}
