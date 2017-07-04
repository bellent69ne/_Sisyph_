#ifndef TWOFISH_HPP
#define TWOFISH_HPP

#include <fstream>
#include "blockCipher.hpp"
#include "twofish.h"
#include "files.h"
#include "modes.h"
#include "shredder.hpp"


class sisyph::Twofish: public sisyph::BlockCipher {
private:
    // Shredder from teenage mutant ninja turtles;)
    // Just kidding. Object that shreds original files
    // when they are encrypted or decrypted
    Shredder m_shredder;
    std::ofstream m_loggedFiles;

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
               For example CBC_Mode<CryptoPP::Twofish>::Encryption for encryption.
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
            if (fileExt.empty()) {
                sinkFile.replace_extension("");
            }

            // Process file
            FileSource file(
                m_currentPath.generic_string().c_str(), true,
                new StreamTransformationFilter(blackOps,
                    new FileSink((sinkFile.generic_string()
                                  + fileExt).c_str()
                    )
                )
            );

            m_loggedFiles << m_currentPath << '\n';

        } catch (const CryptoPP::Exception& exception) {
            std::cerr << exception.what() << std::endl;
        }

        m_shredder.shredFile(m_currentPath);
    }

public:
    // Default construction
    Twofish();

    /* perfect forwarding constructor
       It should be straightforward. If it's not,
       refer to template metaprogramming on your search engine.
    */
    template<typename pathT,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::Twofish,
                std::decay_t<pathT>
            >::value && std::is_constructible<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    explicit Twofish(pathT&& fullPath) noexcept:
                BlockCipher(std::forward<pathT>(fullPath),
                            CryptoPP::Twofish::MAX_KEYLENGTH,
                            CryptoPP::Twofish::BLOCKSIZE),
                m_shredder(),
                m_loggedFiles("loggedFiles.dat") {
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
