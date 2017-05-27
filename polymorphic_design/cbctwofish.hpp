#ifndef CBCTWOFISH_HPP
#define CBCTWOFISH_HPP

#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "twofish.h"
using CryptoPP::Twofish;

#include "filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::StreamTransformationFilter;

#include "hex.h"
using CryptoPP::HexDecoder;

//#include "files.h"
//using CryptoPP::FileSource;
//using CryptoPP::FileSink;

//#include "modes.h"
//using CryptoPP::CBC_Mode;
#include "blockCipher.hpp"

class CBCTwofish: public BlockCipher {
public:

    /*Requires som revision to m_keyLength*/
    CBCTwofish();

    template<typename T,
             typename = std::enable_if_t<
                            !std::is_base_of<
                                 CbcTwofish,
                                 std::decay_t<T>
                             >::value &&
                             std::is_constructible<
                                 path,
                                 std::decay_t<T>
                             >::value
                        >
    >
    explicit inline CBCTwofish(T&& fullPath) noexcept: BlockCipher(fullPath) {
    }


    virtual void encrypt() override;

    virtual void decrypt() override;

    
    inline virtual auto keyLength() noexcept {
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

    virtual void generateKey() override;

    virtual void generateIV() override;

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
    virtual void setKey(keyT&& newKey) override {    // Probably it's not noexcept
        auto keyWithIV(std::forward<keyT>(newKey));
        //m_encKey = std::forward<keyT>(newKey);

        if(keyWithIV.length() != Twofish::MAX_KEYLENGTH * 3) {
            if(exists(newKey)) {
                auto keyFile(static_cast<std::ifstream>(keyWithIV));
                keyWithIV.clear();
                keyFile >> keyWithIV;
                keyFile.close();
            }
            else if(!exists(newKey)) {
                auto keyFilePath(static_cast<path>(currentPath()));
                keyFilePath /= keyWithIV;
                auto keyFile(static_cast<std::ifstream>(keyFilePath.generic_string()));
                keyWithIV.clear();
                keyFile >> keyWithIV;
                keyFile.close();
            }
            else {
                std::cerr << "Invalid key is specified...\n";
                exit(1);
            }
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
    }

    template<typename ivT,  
             typename = std::enable_if_t<
                            std::is_assignable<
                                std::string,
                                std::decay_t<ivT>
                            >::value
                        >
    >
    virtual void setIV(ivT&& newIV) override {
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

        if(!m_byteIV)
            m_byteIV = std::make_unique<byte[]>(Twofish::BLOCKSIZE);

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
