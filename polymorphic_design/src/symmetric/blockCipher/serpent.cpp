#include "serpent.hpp"

// Default constructed mode
sisyph::Serpent::Serpent():
            BlockCipher(
                "",
                "CBC",
                CryptoPP::Serpent::MAX_KEYLENGTH,
                CryptoPP::Serpent::BLOCKSIZE
            ) {
}

// encryption implementation
void sisyph::Serpent::encrypt() {
    /*
        if extension of the processing file is .sisyph,
        skit it, cause that means that it is encrypted.
        We don't want to encrypt file twice
    */
    if(m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /*
        process file. Passing encryption template type parameter.
        Template type parameter specifies the kind of the operation,
        and block cipher mode of operation.
    */
    if(m_blockCipherModes["CBC"])
        process<CBC_Mode<CryptoPP::Serpent>::Encryption>(true);
    // if m_blockCipherModes is ECB, process it with ECB cipher mode
    // ECB is not recommended. Supported just optionally
    else if(m_blockCipherModes["ECB"])
        process<ECB_Mode<CryptoPP::Serpent>::Encryption>(true);
    // if m_blockCipherModesis CTR, process it with CTR cipher mode
    else if(m_blockCipherModes["CTR"])
        process<CTR_Mode<CryptoPP::Serpent>::Encryption>(true);
    // Following are authenticated encryption schemes
    // if m_blockCipherModes is GCM, process it with GCM cipher mode
    else if(m_blockCipherModes["GCM"])
        processAuthentic<GCM<CryptoPP::Serpent>::Encryption>(true);
    // CCM is not supported for now
    // if m_blockCipherModes is EAX, process it with EAX cipher mode
    else if(m_blockCipherModes["EAX"])
        processAuthentic<EAX<CryptoPP::Serpent>::Encryption>(true);

    else {
        std::cerr << "Block cipher mode doesn't exist\n";
        exit(-1);
    }
}

// decryption implementation
void sisyph::Serpent::decrypt() {
    /*
        if extenstion of the processing file is not .sisyph
        skip it, cause we that means that it is decrypted.
        We don't want to decrypt file twice
    */
    if(m_currentPath.extension() != ".sisyph") {
        std::cerr << "Already decrypted... Skipping --------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /*
        process file. Passing decryption template type parameter.
        Template type parameter specifies the kind of the operation,
        and block cipher mode of operation.
    */
    if(m_blockCipherModes["CBC"])
        process<CBC_Mode<CryptoPP::Serpent>::Decryption>(false);
    // if m_blockCipherModes is ECB, process it with ECB cipher mode
    else if(m_blockCipherModes["ECB"])
        process<ECB_Mode<CryptoPP::Serpent>::Decryption>(false);
    // if m_blockCipherModes is CTR, process it with CTR cipher mode
    else if(m_blockCipherModes["CTR"])
        process<CTR_Mode<CryptoPP::Serpent>::Decryption>(false);
    // Following are authenticated Decryption schemes
    // if m_blockCipherModes is GCM, process it with GCM cipher mode
    else if(m_blockCipherModes["GCM"])
        processAuthentic<GCM<CryptoPP::Serpent>::Decryption>(false);
//    // if m_blockCipherModes is CCM, process it with CCM cipher mode
//    else if(m_blockCipherModes == "CCM")
//        process<CCM<CryptoPP::Serpent>::Decryption>(".sisyph");
    // if m_blockCipherModes is EAX, process it with EAX cipher mode
    else if(m_blockCipherModes["EAX"])
        processAuthentic<EAX<CryptoPP::Serpent>::Decryption>(false);

    else {
        std::cerr << "Block cipher mode doesn't exist\n";
        exit(-1);
    }
}

void sisyph::Serpent::generateKey() {
    // Generate encryption key
    generate(m_byteKey, m_byteKey.size(), m_encKey);

    // Now we've gotta generate initialization vector(IV), it's important
    // if we're processing data in ECB mode, then we don't need IV
    if(!m_blockCipherModes["ECB"])
        generateIV();
}

// initialization vector(IV) generation. Both hex encoded and decoded(in bytes);
void sisyph::Serpent::generateIV() {
    // Generate initialization vector(IV)
    generate(m_byteIV, m_byteIV.size(), m_encIV);
}

/* Get the final key representation. It consists of hex encoded key
   appended with hex encoded IV
*/
std::string sisyph::Serpent::getKey() noexcept {
    // ECB mode doesn't use IV, so we don't need it if ECB mode enabled
    if(m_blockCipherModes["ECB"])
        return m_encKey;
    return m_encKey + m_encIV;
}

/* sets encryption key and IV. newKey is hex encoded key appended with hex
   encoded IV, which represent the overall key, or, newKey is just a filename
   with sequence of concatenated encryption key and IV
*/
void sisyph::Serpent::setKey(std::string& newKey) {
    // process new key. Set both, key and IV.
    processKey(newKey);
}
