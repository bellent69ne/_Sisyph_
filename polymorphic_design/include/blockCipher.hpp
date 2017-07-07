#ifndef BLOCKCIPHER_HPP
#define BLOCKCIPHER_HPP

#include "symmetric.hpp"
#include "secblock.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "eax.h"
#include "gcm.h"
#include "shredder.hpp"
#include <map>
#include <iostream>
using namespace CryptoPP;

class sisyph::BlockCipher: public sisyph::Symmetric {
protected:
    // block cipher mode, for example CBC
    // block cipher modes, true if enabled
    // possibly has compiler warnings
    std::map<std::string, bool> m_blockCipherModes; //{
//        {"CBC", false}, {"ECB", false}, {"CTR", false},
//        {"GCM", false}, {"CCM", false}, {"EAX", false}
//    }
    //const std::string m_blockCipherMode;
    SecByteBlock m_byteKey;
    std::string m_encIV;
    byte *m_byteIV;
    char m_blockSize;
    const char m_keyLength;
    // Shredder from teenage mutant ninja turtles;)
    // Just kidding. Object that shreds original files
    // when they are encrypted or decrypted
    Shredder m_shredder;

    void processKey(std::string& newKey);

    /* Perfect forwards path for processing(encryption/decryption).
       Universal for both, encryption and decryption.
       Template type parameter decides which kind of operation
       will be implemented.
    */
    template<typename activityT//, typename extensionT,
        //typename = std::enable_if_t<
        //    std::is_constructible<
        //        std::string,
        //        std::decay_t<extensionT>
        //    >::value
        //>
    >
    void process(bool wannaEncrypt) {//(extensionT&& fileExtension) {
        try {
            /* activityT will decide what kind of operation
               will be performed.
               For example CBC_Mode<CryptoPP::RC6>::Encryption for encryption.
               Look at encrypt() method.
            */
            activityT blackOps;

            // Set key and IV
            if(!m_blockCipherModes["ECB"])
                blackOps.SetKeyWithIV(
                    m_byteKey, m_byteKey.size(), m_byteIV
                );
            else
                blackOps.SetKey(m_byteKey, m_byteKey.size());

            /* extenstionT could possibly be anything which contain characters.
               Make sure we're dealing with string. fileExtension
               is just an extension of the file which will be produced
               while processing original file. If encryption, fileExtension
               will hold ".sisyph" which marks that this file is encrypted
            */
            //auto fileExt(
            //    static_cast<std::string>(
            //        std::forward<extensionT>(fileExtension)
            //    )
            //);

            // sink file is the file which will hold encrypted data
            auto sinkFile(m_currentPath);

            // if we passed empty extenstion, that means that we don't want
            // encrypt that file(decryption will be performed).
            //if(fileExt.empty())
            //    sinkFile.replace_extension("");
            wannaEncrypt ? sinkFile.replace_extension(".sisyph")
                         : sinkFile.replace_extension("");

            // Process file
            FileSource file(
                m_currentPath.generic_string().c_str(), true,
                new StreamTransformationFilter(blackOps,
                    new FileSink(sinkFile.generic_string().c_str()
                                  //+ fileExt).c_str()
                    )
                )
            );
        }
        catch(const CryptoPP::Exception& e) {
            std::cerr << "\nCouldn't process "
                      << m_currentPath.generic_string() << std::endl;
            std::cerr << e.what() << std::endl;
        }

        m_shredder.shredFile(m_currentPath);
    }

    // processing method for authenticated modes of block cipher of operations
    template<typename activityT>
    void processAuthentic(bool wannaEncrypt) {
        try {
            /*  activityT will decide what kind of operation
                will be performed.
                For example CBC_Mode<CryptoPP::RC6>::Encryption for encryption.
                Look at encrypt() method.
            */
            activityT blackOps;

            // Set key and IV
            // Possibly doesn't work properly with EAX mode
            blackOps.SetKeyWithIV(m_byteKey, m_byteKey.size(),
                                  m_byteIV, m_blockSize);

            // sink file is the file which will hold encrypted data
            auto sinkFile(m_currentPath);

            // if we passed empty extenstion, that means that we don't want
            // encrypt that file(decryption will be performed).
            //if(fileExt.empty())
            //    sinkFile.replace_extension("");
            wannaEncrypt ? sinkFile.replace_extension(".sisyph")
                         : sinkFile.replace_extension("");

            // if encryption is taking place, then
            // use AuthenticatedEncryptionFilter(for encryption)
            if(wannaEncrypt)
                FileSource file(
                    m_currentPath.generic_string().c_str(), true,
                    new AuthenticatedEncryptionFilter(blackOps,
                        new FileSink(sinkFile.generic_string().c_str()
                        )
                    )
                );
            // otherwise, use AuthenticatedDecryptionFilter(for decryption)
            else
                FileSource file(
                    m_currentPath.generic_string().c_str(), true,
                    new AuthenticatedDecryptionFilter(blackOps,
                        new FileSink(sinkFile.generic_string().c_str()
                        )
                    )
                );
        }
        catch(const CryptoPP::Exception& e) {
            std::cerr << "\nUnauthorized modification!\n";
            std::cerr << e.what() << std::endl;
        }
        // Destroy original file
        m_shredder.shredFile(m_currentPath);
    }

public:
    template<typename pathT, typename cipherMode,
        typename = std::enable_if_t<
            !std::is_base_of<
                BlockCipher,
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
    explicit BlockCipher(pathT&& fullPath, cipherMode&& blockCipherMode,
                         char keyLength, char blockSize):
            Symmetric(std::forward<pathT>(fullPath)),
        //    m_blockCipherMode(std::forward<cipherMode>(blockCipherMode)),
            m_blockCipherModes {     // compiler possibly warns about {}
                {
                    {"CBC", false}, {"ECB", false}, {"CTR", false},
                    {"GCM", false}, {"CCM", false}, {"EAX", false}
                }
            },
            m_byteKey(keyLength),
            m_encIV(""),
            m_byteIV(new byte[blockSize]),
            m_blockSize(blockSize),
            m_keyLength(keyLength),
            m_shredder() {
        if(m_blockCipherModes.find(blockCipherMode) !=
           m_blockCipherModes.end())
           m_blockCipherModes[blockCipherMode] = true;
    }

    virtual void generateIV() = 0;

    inline virtual  std::string getIV() noexcept {
        return m_encIV;
    }

    template<typename ivT,
        typename = std::enable_if_t<
            std::is_assignable<
                std::string,
                std::decay_t<ivT>
            >::value
        >
    >
    void setIV(ivT&& newIV) {
        m_encIV = std::forward<ivT>(newIV);

        auto decodedIV(static_cast<std::string>(""));
        StringSource resetIV(m_encIV, true,
            new HexDecoder(
                new StringSink(decodedIV)
            )
        );

        auto convertToByte([&m_byteIV = m_byteIV, &decodedIV] {
            auto iii(0);
            for(auto& each: decodedIV) {
                m_byteIV[iii] = each;
                ++iii;
            }
        });

        convertToByte();
    }


    virtual char keyLength() noexcept override;

    virtual ~BlockCipher();

};

#endif
