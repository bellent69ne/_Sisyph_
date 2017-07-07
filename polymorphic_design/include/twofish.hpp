#ifndef TWOFISH_HPP
#define TWOFISH_HPP

#include "blockCipher.hpp"
#include "twofish.h"

class sisyph::Twofish: public sisyph::BlockCipher {
public:
    // Default construction
    Twofish();

    // perfect forwarding constructor. Forwards only block cipher mode
    template<typename cipherMode,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::RC6,
                std::decay_t<cipherMode>
            >::value &&
            std::is_constructible<
                std::string,
                std::decay_t<cipherMode>
            >::value
        >
    >
    Twofish(cipherMode&& blockCipherMode):
            BlockCipher("", std::forward<cipherMode>(blockCipherMode),
                            CryptoPP::Twofish::MAX_KEYLENGTH,
                            CryptoPP::Twofish::BLOCKSIZE) {
    }

    /* perfect forwarding constructor
       It should be straightforward. If it's not,
       refer to template metaprogramming on your search engine.
    */
    template<typename pathT, typename cipherMode,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::Twofish,
                std::decay_t<pathT>
            >::value &&
            std::is_constructible<
                filesystem::path,
                std::decay_t<pathT>
            >::value &&
            std::is_constructible<
                std::string,
                std::decay_t<cipherMode>
            >::value
        >
    >
    explicit Twofish(pathT&& fullPath, cipherMode&& blockCipherMode) noexcept:
                BlockCipher(std::forward<pathT>(fullPath),
                            std::forward<cipherMode>(blockCipherMode),
                            CryptoPP::Twofish::MAX_KEYLENGTH,
                            CryptoPP::Twofish::BLOCKSIZE) {
    }

    // main implementation of encryption
    virtual void encrypt() override;

    // main implementation of decryption
    virtual void decrypt() override;

    // key generation method(256 bit)
    virtual void generateKey() override;

    // initialization vector(IV) generation method
    virtual void generateIV() override;

    // getter for m_encKey. Simply returns encryption key in encoded hex format
    virtual std::string getKey() noexcept override;

    // setter for m_encKey. Sets new key from encoded hex format
    // Then decodes it to byte format
    virtual void setKey(std::string& newKey) override;
};


#endif
