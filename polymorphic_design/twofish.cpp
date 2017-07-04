#include <vector>
#include <cstdlib>

#include "osrng.h"
#include "cbctwofish.hpp"

// Default constructed mode,
CBCTwofish::CBCTwofish():
                        Block_Cipher("", Twofish::MAX_KEYLENGTH,
                                     Twofish::BLOCKSIZE),
                        m_shredder(),
                        m_loggedFiles("loggedFiles.dat") {
}


void CBCTwofish::encrypt() {
    if (m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath << std::endl;
        return;
    }

    /* We definitely can choose cipher modes */
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

    std::cout << m_encKey << std::endl;

    generateIV();
}

void CBCTwofish::generateIV() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteIV, Twofish::BLOCKSIZE);

    m_encIV.clear();
    StringSource ssIV(m_byteIV, Twofish::BLOCKSIZE, true,
        new HexEncoder (
            new StringSink(m_encIV)
        )
    );

    std::cout << m_encIV << std::endl;
}

void CBCTwofish::setKey(const std::string& newKey) {
    auto keyWithIV(newKey);
    //m_encKey = std::forward<keyT>(newKey);

    if (keyWithIV.length() != Twofish::MAX_KEYLENGTH * 3) {
        if(filesystem::exists(newKey)) {
            std::ifstream keyFile(keyWithIV);

            keyWithIV.clear();
            keyFile >> keyWithIV;
            keyFile.close();
        }
        else if (!filesystem::exists(newKey)) {
            auto keyFilePath(
                static_cast<filesystem::path>(currentPath())
            );

            keyFilePath /= keyWithIV;
            std::ifstream keyFile(keyFilePath.generic_string());

            keyWithIV.clear();
            keyFile >> keyWithIV;
            keyFile.close();
        }
        else {
            std::cerr << "Invalid key is specified..." << std::endl;
            exit(1);
        }
    }

    m_encKey.assign(keyWithIV.cbegin(), keyWithIV.cend() - 32);

    auto decodedKey(static_cast<std::string>(""));
    StringSource resetKey(m_encKey, true,
        new HexDecoder(
            new StringSink(decodedKey)
        )
    );

    m_byteKey.Assign((byte*) decodedKey.data(), decodedKey.size());

    keyWithIV.assign(keyWithIV.cend() - 32, keyWithIV.cend());

    setIV(std::move(keyWithIV));
}
