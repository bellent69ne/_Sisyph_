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
#include "hex.h"
using CryptoPP::HexDecoder;

class Cryptographer {
private:
    path m_currentPath;
    int m_keyLength;
    std::string m_encKey;
    std::string m_encIV;
    std::unique_ptr<SecByteBlock> m_byteKey;
    byte m_byteIV[Twofish::BLOCKSIZE];

public:
    /*Requires som revision to m_keyLength*/
    Cryptographer();

    /*May be we should check for std::is_constructible?*/
    template<typename T,
             typename = std::enable_if_t<
                            !std::is_base_of<Cryptographer,
                                             std::decay_t<T>
                             >::value
                        >
    >
    explicit inline Cryptographer(T&& fullPath) noexcept:
                        m_currentPath(std::forward<T>(fullPath)),
                        m_keyLength(0),
                        m_encKey(""),
                        m_encIV("") {
    }

    void encrypt() const;

    template<typename T,
             typename = std::enable_if_t<
                            std::is_assignable<path,
                                    std::remove_reference_t<T>
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
                            std::is_assignable<path,
                                std::remove_reference_t<T>
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
        return m_keyLength;
    }

    template<typename T,
             typename = std::enable_if_t<
                            std::is_assignable<path, 
                                       std::remove_reference_t<T>
                            >::value
                        >
    >
    inline void setPath(T&& newPath) noexcept {
        m_currentPath = std::forward<T>(newPath);
    } // Probably noexcept too

    void generateKey();

    template<typename T,
             typename = std::enable_if_t<
                            std::is_integral<T>::value
                        >
    >
    void generateKey(T&& keyLength) {
        m_keyLength = std::forward<T>(keyLength);
        generateKey();
    }

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
                                std::remove_reference_t<keyT>
                            >::value
                        >
    >
    void setKey(keyT&& newKey) noexcept {
        m_encKey = std::forward<keyT>(newKey);

        //if()
        auto decodedKey(static_cast<std::string>(""));
        StringSource resetKey(m_encKey, true,
            new HexDecoder(
                new StringSink(decodedKey)
            )
        );

        // Yet needs some consideration
        m_byteKey->Assign((byte*) decodedKey.data(), decodedKey.size());

        //std::cout << "decoded " << decodedKey << std::endl;
    }

    template<typename ivT,  
             typename = std::enable_if_t<
                            std::is_assignable<
                                std::string,
                                std::remove_reference_t<ivT>
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
};

#endif
