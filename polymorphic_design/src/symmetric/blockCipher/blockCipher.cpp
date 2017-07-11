#include "blockCipher.hpp"

/* Universal key and IV setter. Takes filename or key sequence
   which consists of concatenated key and IV.
*/
void sisyph::BlockCipher::processKey(std::string& newKey) {
    // copy newKey
    auto keyWithIV(newKey);
    // We definitely know the size of encryption key
    // which is 32 bytes or 256 bits

    // if it's not the actual concatenated key and IV sequence
    // (concatenated key and IV sequence is 96 characters long)
    if (keyWithIV.length() != m_keyLength * 3) {
        // if it's file
        if(filesystem::exists(keyWithIV)) {
            // open that file
            std::ifstream keyFile(keyWithIV);

            // clear keyWithIV to read there concatenated key and IV from
            // that file
            keyWithIV.clear();
            // read secret key appended with IV into keyWithIV
            keyFile >> keyWithIV;
            // close that file
            keyFile.close();
        }
        // if it's not a file, possibly it's a filename stored
        // in current path
        else if (!filesystem::exists(keyWithIV)) {
            // determine current path
            auto keyFilePath(
                static_cast<filesystem::path>(currentPath())
            );
            // append a filename to it to create a full path to a file
            // which possibly contains secret key appended with IV
            keyFilePath /= keyWithIV;
            // open that file
            std::ifstream keyFile(keyFilePath.generic_string());

            // clear keyWithIV to read there concatenated key and IV from
            // that file
            keyWithIV.clear();

            // read secret key appended with IV into keyWithIV
            keyFile >> keyWithIV;
            // close that file
            keyFile.close();
        }
        // Well, otherwise, there's nothing which even looks like a key
        else {
            std::cerr << "Invalid key is specified..." << std::endl;
            exit(1);
        }
    }
    // Well, if it's not a file, then it's definitely the key sequence itself
    m_encKey.assign(keyWithIV.cbegin(), keyWithIV.cbegin() + 64);

    // create a storage to store our secret key in decoded format
    // (in bytes)
    auto decodedKey(static_cast<std::string>(""));
    // decode hex encoded key
    StringSource resetKey(m_encKey, true,
        new HexDecoder(
            new StringSink(decodedKey)
        )
    );

    // convert it into bytes, and store that byte sequence in m_byteKey
    m_byteKey.Assign((byte*) decodedKey.data(), decodedKey.size());

    // ECB mode doesn't use IV, we want to setIV only if our mode of operation
    // needs IV, but for ECB, we don't want to set it
    if(!m_blockCipherModes["ECB"]) {
        // now just do the same thing with IV. Store only IV sequence in keyWithIV
        keyWithIV.assign(keyWithIV.cend() - 32, keyWithIV.cend());
        // move it to setIV in order to generate IV
        setIV(std::move(keyWithIV));
    }
}

// get the length of encryption key
char sisyph::BlockCipher::keyLength() noexcept {
    return m_keyLength;
}

// Destroy BlockCipher object
/*sisyph::BlockCipher::~BlockCipher() {
    // if we have a byte sequence of IV in heap
    if(m_byteIV) {
        // we definitely want to zeroize it, to erase potential IV leak
        for(auto iii(0); iii < m_blockSize; ++iii)
            m_byteIV[iii] = 0;
        // and then just release that memory
        delete[] m_byteIV;
        m_byteIV = nullptr;
    }
}*/
