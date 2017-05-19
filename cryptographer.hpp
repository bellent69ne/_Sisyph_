#ifndef CRYPTOGRAPHER_HPP
#define CRYPTOGRAPHER_HPP
#include <string>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "secblock.h"
using CryptoPP::SecByteBlock;

#include "twofish.h"
using CryptoPP::Twofish;

#include "filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::StreamTransformationFilter;

#include "hex.h"
using CryptoPP::HexDecoder;

#include "files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;

#include "modes.h"
using CryptoPP::CBC_Mode;

class Cryptographer {
private:
    path m_currentPath;
    //int m_keyLength,
    std::string m_encKey;
    std::string m_encIV;
    SecByteBlock m_byteKey;
    byte m_byteIV[Twofish::BLOCKSIZE];
    bool m_goingToEncrypt;
    //std::unique_ptr<CBC_Mode<Twofish>::Encryption> m_twEncrypt;
    //std::unique_ptr<CBC_Mode<Twofish>::Decryption> m_twDecrypt;

    
        
    /*template<typename processT, std::enable_if<
                                    std::is_pointer<processT>::value
                                    //&&
                                    //std::is_assignable<CBC_Mode<Twofish>::Encryption,
                                      //  std::decay_t<processT>
                                    //>::value
                                >
    >
    void process(processT* cbcTwofish,  bool isEncrypted) {
        auto encryptedFilename([&isEncrypted, &m_currentPath = m_currentPath] {
            //isEncrypted ? (return m_currentPath.generic_string() + ".Sisyph")
              //          : (return  m_currentPath.generic_string());
            if(isEncrypted)
                return m_currentPath.generic_string()  + ".Sisyph";
            else
                return m_currentPath.generic_string();
        });

        try {
            //CBC_Mode<Twofish>::Encryption cbcTwofish;
            cbcTwofish->SetKeyWithIV(*m_byteKey, m_byteKey->size(),
                                     m_byteIV);
            FileSource file(m_currentPath.generic_string().c_str(), true,
                new StreamTransformationFilter(*cbcTwofish,
                    new FileSink(encryptedFilename().c_str()
                    )
                )
            );

        } 
        catch(const CryptoPP::Exception& e) {
            std::cerr << e.what() << std::endl;
            exit(1);
        }
    }*/

  /*  auto fileToShred() const {
        auto forShredding(m_currentPath.generic_string());

        auto itr(forShredding.cbegin());

        while(itr != forShredding.cend()) {
            if(*itr == ' ' && *(itr - 1) != '\\')
                forShredding.insert(itr, '\\');

            ++itr;
        }

        return forShredding;
    }
*/

public:

    /*Requires som revision to m_keyLength*/
    Cryptographer();

    /*May be we should check for std::is_constructible?*/
    template<typename T,
             typename = std::enable_if_t<
                            !std::is_base_of<
                                 Cryptographer,
                                 std::decay_t<T>
                             >::value &&
                             std::is_constructible<
                                 path,
                                 std::decay_t<T>
                             >::value
                        >
    >
    explicit inline Cryptographer(T&& fullPath) noexcept:
                        m_currentPath(std::forward<T>(fullPath)),
                       // m_keyLength(Twofish::MAX_KEYLENGTH),
                        m_encKey(""),
                        m_encIV(""),
                        m_goingToEncrypt(true) {
    }


    void encrypt() const;

    template<typename T,
             typename = std::enable_if_t<
                            std::is_assignable<
                                path,
                                std::decay_t<T>
                            >::value
                        >
    >
    void encrypt(T&& fullPath) {
        m_currentPath = std::forward<T>(fullPath);

        encrypt();
    }

    void decrypt() const;

    template<typename T,
             typename = std::enable_if_t<
                            std::is_assignable<
                                path,
                                std::decay_t<T>
                            >::value
                        >
    >
    void decrypt(T&& fullPath) {
        m_currentPath = std::forward<T>(fullPath);

        decrypt();
    }


    inline auto currentPath() noexcept {
        return m_currentPath.generic_wstring();
    }
    
    inline auto keyLength() noexcept {
        //if(m_keyLength < 0)
          //  m_keyLength = 0;
        return Twofish::MAX_KEYLENGTH;
    }

    template<typename T,
             typename = std::enable_if_t<
                            std::is_assignable<
                                path, 
                                std::decay_t<T>
                            >::value
                        >
    >
    inline void setPath(T&& newPath) noexcept {
        m_currentPath = std::forward<T>(newPath);
    } // Probably noexcept too

    void generateKey();

    /*template<typename T,
             typename = std::enable_if_t<
                            std::is_integral<T>::value
                        >
    >
    void generateKey(T&& keyLength) {
        m_keyLength = std::forward<T>(keyLength);
        generateKey();
    }*/

    void generateIV();

    inline auto getKey() noexcept{
        return m_encKey;
    }

    inline auto getIV() noexcept{
        return m_encIV;
    }

    template<typename keyT, 
             typename = std::enable_if_t<
                            std::is_assignable<
                                std::string,
                                std::decay_t<keyT>
                            >::value
                        >
    >
    void setKey(keyT&& newKey) {    // Probably it's not noexcept
        auto keyWithIV(std::forward<keyT>(newKey));
        //m_encKey = std::forward<keyT>(newKey);

        if(keyWithIV.length() != Twofish::MAX_KEYLENGTH * 3) {
            std::cerr << "Invalid key is specified...\n";
            exit(1);
        }

        m_encKey.assign(keyWithIV.cbegin(), keyWithIV.cend() - 32);

        auto decodedKey(static_cast<std::string>(""));
        StringSource resetKey(m_encKey, true,
            new HexDecoder(
                new StringSink(decodedKey)
            )
        );

        //generateKey(m_encKey.length() / 2);

        // Yet needs some consideration
        m_byteKey.Assign((byte*) decodedKey.data(), decodedKey.size());

        keyWithIV.assign(keyWithIV.cend() - 32, keyWithIV.cend());

        setIV(std::move(keyWithIV));
        /*auto convertToByte([&m_byteKey = m_byteKey, &decodedKey] {
            std::vector<byte> byteKey;
            for(auto& each: decodedKey) {
                byteKey.push_back(each);
            }

            m_byteKey->Assign(byteKey.data(), byteKey.size());
        });

        convertToByte();*/

        //std::cout << "decoded " << decodedKey << std::endl;
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

        /*if(m_encIV.length() != Twofish::BLOCKSIZE) {
            std::cerr << "IV is less than blocksize...";
            exit(1);
        }*/

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

    // Requires consideration
    inline bool goingToEncrypt() const {
        return m_goingToEncrypt;
    }

    inline void willEncrypt(bool trueOrFalse) {
        m_goingToEncrypt = trueOrFalse;
    }
};

#endif
