#ifndef RC6_HPP
#define RC6_HPP

#include "blockCipher.hpp"
#include "shredder.hpp"
#include "files.h"
#include "modes.h"
#include "rc6.h"

class sisyph::RC6: public sisyph::BlockCipher {
private:
    // Shredder from teenage mutant ninja turtles;)
    // Just kidding. Object that shreds original files
    // when they are encrypted or decrypted
    Shredder m_shredder;

    /* Perfect forwards path for processing(encryption/decryption).
       Universal for both, encryption and decryption.
       Template type parameter decides which kind of operation
       will be implemented.
    */
    template<typename activityT, typename extensionT,
        typename = std::enable_if_t<
            std::is_constructible<
                std::string,
                std::decay_t<extensionT>
            >::value
        >
    >
    void process(extensionT&& fileExtension) {
        try {
            /* activityT will decide what kind of operation
               will be performed.
               For example CBC_Mode<CryptoPP::RC6>::Encryption for encryption.
               Look at encrypt() method.
            */
            activityT blackOps;

            // Set key and IV
            blackOps.SetKeyWithIV(
                m_byteKey, m_byteKey.size(), m_byteIV
            );

            /* extenstionT could possibly be anything which contain characters.
               Make sure we're dealing with string. fileExtension
               is just an extension of the file which will be produced
               while processing original file. If encryption, fileExtension
               will hold ".sisyph" which marks that this file is encrypted
            */
            auto fileExt(
                static_cast<std::string>(
                    std::forward<extensionT>(fileExtension)
                )
            );

            // sink file is the file which will hold encrypted data
            auto sinkFile(m_currentPath);

            // if we passed empty extenstion, that means that we don't want
            // encrypt that file(decryption will be performed).
            if(fileExt.empty())
                sinkFile.replace_extension("");

            // Process file
            FileSource file(
                m_currentPath.generic_string().c_str(), true,
                new StreamTransformationFilter(blackOps,
                    new FileSink((sinkFile.generic_string()
                                  + fileExt).c_str()
                    )
                )
            );
        }
        catch(const CryptoPP::Exception& e) {
            std::cerr << e.what() << std::endl;
        }

        m_shredder.shredFile(m_currentPath);
    }

public:
    // Default construction
    RC6();

    /* perfect forwarding constructor
       It should be straightforward. If it's not,
       refer to template metaprogramming on your search engine.
    */
    template<typename pathT,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::RC6,
                std::decay_t<pathT>
            >::value &&
            std::is_constructible<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    explicit RC6(pathT&& fullPath) noexcept:
                BlockCipher(std::forward<pathT>(fullPath),
                            CryptoPP::RC6::MAX_KEYLENGTH,
                            CryptoPP::RC6::BLOCKSIZE),
                m_shredder() {
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
