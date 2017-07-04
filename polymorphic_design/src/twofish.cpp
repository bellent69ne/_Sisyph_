#include <vector>
#include <cstdlib>

#include "osrng.h"
#include "twofish.hpp"

// Default constructed mode,
sisyph::Twofish::Twofish():
                        BlockCipher("", CryptoPP::Twofish::MAX_KEYLENGTH,
                                        CryptoPP::Twofish::BLOCKSIZE),
                        m_shredder(),
                        m_loggedFiles("loggedFiles.dat") {
}


void sisyph::Twofish::encrypt() {
    if (m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath << std::endl;
        return;
    }

    /* We definitely can choose cipher modes */
    process<CBC_Mode<CryptoPP::Twofish>::Encryption>(".sisyph");
}

void sisyph::Twofish::decrypt() {
    if (m_currentPath.extension() != ".sisyph") {
        std::cerr << "Already decrypted... Skipping -------> "
                  << m_currentPath << std::endl;
        return;
    }

    process<CBC_Mode<CryptoPP::Twofish>::Decryption>("");
}


void sisyph::Twofish::generateKey() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteKey, m_byteKey.size());

    m_encKey.clear();
    StringSource ssKey(m_byteKey, m_byteKey.size(), true,
        new HexEncoder (
            new StringSink(m_encKey)
        )
    );

    generateIV();
}

void sisyph::Twofish::generateIV() {
    AutoSeededRandomPool prng;
    prng.GenerateBlock(m_byteIV, CryptoPP::Twofish::BLOCKSIZE);

    m_encIV.clear();
    StringSource ssIV(m_byteIV, CryptoPP::Twofish::BLOCKSIZE, true,
        new HexEncoder (
            new StringSink(m_encIV)
        )
    );
}

void sisyph::Twofish::setKey(const std::string& newKey) {
    auto keyWithIV(newKey);
    //m_encKey = std::forward<keyT>(newKey);

    if (keyWithIV.length() != CryptoPP::Twofish::MAX_KEYLENGTH * 3) {
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
