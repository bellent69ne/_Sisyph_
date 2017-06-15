#include <vector>
#include <cstdlib>

#include "osrng.h"
#include "cbctwofish.hpp"

using CryptoPP::AutoSeededRandomPool;

// Default constructed mode,
CBCTwofish::CBCTwofish(): m_currentPath(""),
                                m_encKey(""),
                                m_encIV(""),
                                m_byteKey(Twofish::MAX_KEYLENGTH),
                                m_willEncrypt(true),
                                m_loggedFiles("loggedFiles.dat") {
}


void CBCTwofish::encrypt() {
    if (m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath << std::endl;
        return;
    }

    process<CBC_Mode<Twofish>::Encryption>(".sisyph");
}


void CBCTwofish::decrypt() {
    if (m_currentPath.extension() != ".sisyph") {
        std::cerr << "Already decrypted... Skipping -------> "
                  << m_currentPath << std::endl;
        return;
    }
    
    process<CBC_Mode<Twofish>::Decryption>("");
}


void CBCTwofish::generateKey() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteKey, m_byteKey.size());

    m_encKey.clear();
    StringSource ssKey(m_byteKey, m_byteKey.size(), true,
        new HexEncoder (
            new StringSink(m_encKey)
        )
    );
}


void CBCTwofish::generateIV() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteIV, sizeof(m_byteIV));

    m_encIV.clear();
    StringSource ssIV(m_byteIV, sizeof(m_byteIV), true,
        new HexEncoder (
            new StringSink(m_encIV)
        )
    );
}
