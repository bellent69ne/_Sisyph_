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
CBCTwofish::CBCTwofish(): BlockCipher() {
}



virtual void CBCTwofish::encrypt() override {
    if(m_currentPath.extension() == ".sisyph") {
        std::cerr << "File is already encrypted..." << std::endl;
        exit(1);
    }

    /***********************************************************/

    try {
        CBC_Mode<Twofish>::Encryption cbcTwofish;
        cbcTwofish.SetKeyWithIV(m_byteKey, m_byteKey.size(),
                                m_byteIV);
        FileSource file(m_currentPath.generic_string().c_str(), true,
            new StreamTransformationFilter(cbcTwofish,
                new FileSink((m_currentPath.generic_string() 
                              + ".sisyph").c_str()
                )
            )
        );

    } catch(const CryptoPP::Exception& e) {
          std::cerr << e.what() << std::endl;
          exit(1);
      }
          

      m_shredder.shredFile(m_currentPath);
}

/*****************************************************/

virtual void CBCTwofish::decrypt() override {
    if(m_currentPath.extension() != ".sisyph") {
        std::cerr << "The file is decrypted..." << std::endl;
        exit(1);
    }
    
    /********************************************************/

    try {
        CBC_Mode<Twofish>::Decryption cbcTwofishD;
        cbcTwofishD.SetKeyWithIV(m_byteKey, m_byteKey.size(),
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
 
    m_shredder.shredFile(m_currentPath);
    //  std::cout << fileToShred() << std::endl;
}

virtual void CBCTwofish::generateKey() override {
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

virtual void CBCTwofish::generateIV() override {
    AutoSeededRandomPool prng;

    if(!m_byteIV)
        m_byteIV = std::make_unique<byte[]>(Twofish::BLOCKSIZE);

    prng.GenerateBlock(m_byteIV, sizeof(m_byteIV));

    m_encIV.clear();
    StringSource ssIV(m_byteIV, sizeof(m_byteIV), true,
        new HexEncoder (
            new StringSink(m_encIV)
        )   // For HexEncoder
    );  // For StringSource
}
