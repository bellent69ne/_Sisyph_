#ifndef RC6_HPP
#define RC6_HPP

#include "blockCipher.hpp"
#include "rc6.h"

class sisyph::RC6: public sisyph::BlockCipher {
public:
    // Default construction
    RC6();

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
    RC6(cipherMode&& blockCipherMode):
            BlockCipher("", std::forward<cipherMode>(blockCipherMode),
                            CryptoPP::RC6::MAX_KEYLENGTH,
                            CryptoPP::RC6::BLOCKSIZE) {
    }

    /* perfect forwarding constructor
       It should be straightforward. If it's not,
       refer to template metaprogramming on your search engine.
    */
    template<typename pathT, typename cipherMode,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::RC6,
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
    explicit RC6(pathT&& fullPath, cipherMode&& blockCipherMode) noexcept:
                BlockCipher(std::forward<pathT>(fullPath),
                            std::forward<cipherMode>(blockCipherMode),
                            CryptoPP::RC6::MAX_KEYLENGTH,
                            CryptoPP::RC6::BLOCKSIZE) {
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
