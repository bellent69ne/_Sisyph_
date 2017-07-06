#include "rc6.hpp"

// Default constructed mode
sisyph::RC6::RC6(): BlockCipher("", "CBC",
                                    CryptoPP::RC6::MAX_KEYLENGTH,
                                    CryptoPP::RC6::BLOCKSIZE) {
}

// encryption implementation
void sisyph::RC6::encrypt() {
    /* if extenstion of the processing file is .sisyph
     skip it, cause we that means that it is encrypted.
     We don't want to encrypt file twice
    */
    if(m_currentPath.extension() == ".sisyph") {
        std::cerr << "Already encrypted... Skipping --------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /* process file. Passing encryption template type parameter.
       We can even choose block cipher modes, that will be implemented soon.
    */
    if(m_blockCipherMode == "CBC")
        process<CBC_Mode<CryptoPP::RC6>::Encryption>(".sisyph");
    // if m_blockCipherMode is ECB, process it with ECB cipher mode
    else if(m_blockCipherMode == "ECB")
        process<ECB_Mode<CryptoPP::RC6>::Encryption>(".sisyph");
    // if m_blockCipherMode is CTR, process it with CTR cipher mode
    else if(m_blockCipherMode == "CTR")
        process<CTR_Mode<CryptoPP::RC6>::Encryption>(".sisyph");
    // Following are authenticated encryption schemes
    // if m_blockCipherMode is GCM, process it with GCM cipher mode
    /*else if(m_blockCipherMode == "GCM")
        process<GCM<CryptoPP::RC6>::Encryption>(".sisyph");
    // if m_blockCipherMode is CCM, process it with CCM cipher mode
    else if(m_blockCipherMode == "CCM")
        process<CCM<CryptoPP::RC6>::Encryption>(".sisyph");
    // if m_blockCipherMode is EAX, process it with EAX cipher mode
    else if(m_blockCipherMode == "EAX")
        process<EAX<CryptoPP::RC6>::Encryption>(".sisyph");*/

    else {
        std::cerr << m_blockCipherMode << " block cipher mode doesn't exist\n";
        exit(-1);
    }
}

// decryption implementation
void sisyph::RC6::decrypt() {
    /* if extenstion of the processing file is not .sisyph
     skip it, cause we that means that it is decrypted.
     We don't want to decrypt file twice
    */
    if(m_currentPath.extension() != ".sisyph") {
        std::cerr << "Already decrypted... Skipping -------> "
                  << m_currentPath.generic_string() << std::endl;
        return;
    }

    /* process file. Passing decryption template type parameter,
       just like above.
       We can even choose block cipher modes, that will be implemented soon.
    */
    if(m_blockCipherMode == "CBC")
        process<CBC_Mode<CryptoPP::RC6>::Decryption>("");
    // if m_blockCipherMode is ECB, process it with ECB cipher mode
    else if(m_blockCipherMode == "ECB")
        process<ECB_Mode<CryptoPP::RC6>::Decryption>("");
    // if m_blockCipherMode is CTR, process it with CTR cipher mode
    else if(m_blockCipherMode == "CTR")
        process<CTR_Mode<CryptoPP::RC6>::Decryption>("");
    // Following are authenticated Decryption schemes
    // if m_blockCipherMode is GCM, process it with GCM cipher mode
    /*else if(m_blockCipherMode == "GCM")
        process<GCM<CryptoPP::RC6>::Decryption>(".sisyph");
    // if m_blockCipherMode is CCM, process it with CCM cipher mode
    else if(m_blockCipherMode == "CCM")
        process<CCM<CryptoPP::RC6>::Decryption>(".sisyph");
    // if m_blockCipherMode is EAX, process it with EAX cipher mode
    else if(m_blockCipherMode == "EAX")
        process<EAX<CryptoPP::RC6>::Decryption>(".sisyph");
*/
    else {
        std::cerr << m_blockCipherMode << " block cipher mode doesn't exist\n";
        exit(-1);
    }
}

// encryption key generation. Both hex encoded and decoded(in bytes);
void sisyph::RC6::generateKey() {
    // Generate encryption key
    generate(m_byteKey, m_byteKey.size(), m_encKey);

    // Now we've gotta generate initialization vector(IV), it's important
    generateIV();
}

// initialization vector(IV) generation. Both hex encoded and decoded(in bytes);
void sisyph::RC6::generateIV() {
    // Generate initialization vector(IV)
    generate(m_byteIV, CryptoPP::RC6::BLOCKSIZE, m_encIV);
}

/* Get the final key representation. It consists of hex encoded key
   appended with hex encoded IV
*/
std::string sisyph::RC6::getKey() noexcept {
    return m_encKey + m_encIV;
}

/* sets encryption key and IV. newKey is hex encoded key appended with hex
   encoded IV, which represent the overall key, or, newKey is just a filename
   with sequence of concatenated encryption key and IV
*/
void sisyph::RC6::setKey(std::string& newKey) {
    // process new key. Set both, key and IV.
    processKey(newKey);
}
