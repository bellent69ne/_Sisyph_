#ifndef CBCTWOFISH_HPP
#define CBCTWOFISH_HPP

#include <string>
#include <fstream>
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
using CryptoPP::HexEncoder;

#include "files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;

#include "modes.h"
using CryptoPP::CBC_Mode;

#include "shredder.hpp"


class CBCTwofish {
private:
    path m_currentPath;
    std::string m_encKey;
    std::string m_encIV;
    SecByteBlock m_byteKey;
    byte m_byteIV[Twofish::BLOCKSIZE];
    bool m_willEncrypt;
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
            activityT cbcTwofish;
            cbcTwofish.SetKeyWithIV(
                m_byteKey, m_byteKey.size(), m_byteIV
            );

            auto fileExt(
                static_cast<std::string>(
                    std::forward<extensionT>(fileExtension)
                )
            );
            auto sinkFile(m_currentPath);

            if (fileExt.empty()) {
                sinkFile.replace_extension("");
            }

            FileSource file(
                m_currentPath.generic_string().c_str(), true,
                new StreamTransformationFilter(cbcTwofish,
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
    CBCTwofish();

    template<typename T,
        typename = std::enable_if_t<
            !std::is_base_of<
                CBCTwofish,
                std::decay_t<T>
            >::value && std::is_constructible<
                path,
                std::decay_t<T>
            >::value
        >
    >
    explicit inline CBCTwofish(T&& fullPath) noexcept:
            m_currentPath(std::forward<T>(fullPath)),
            m_encKey(""),
            m_encIV(""),
            m_willEncrypt(true),
            m_loggedFiles("loggedFiles.dat") {
    }


    void encrypt();

    template<typename pathT,
        typename = std::enable_if_t<
            std::is_assignable<
                path,
                std::decay_t<pathT>
            >::value
        >
    >
    void encrypt(pathT&& fullPath) {
        m_currentPath = std::forward<pathT>(fullPath);

        encrypt();
    }

    void decrypt();

    template<typename pathT,
             typename = std::enable_if_t<
                            std::is_assignable<
                                path,
                                std::decay_t<pathT>
                            >::value
                        >
    >
    void decrypt(pathT&& fullPath) {
        m_currentPath = std::forward<pathT>(fullPath);

        decrypt();
    }


    inline auto currentPath() noexcept {
        return m_currentPath.generic_wstring();
    }

    inline auto keyLength() noexcept {
        return Twofish::MAX_KEYLENGTH;
    }

    template<typename pathT,
        typename = std::enable_if_t<
            std::is_assignable<
                path,
                std::decay_t<pathT>
            >::value
        >
    >
    inline void setPath(pathT&& newPath) noexcept {
        m_currentPath = std::forward<pathT>(newPath);
    }

    void generateKey();

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
    void setKey(keyT&& newKey) {
        auto keyWithIV(std::forward<keyT>(newKey));
        //m_encKey = std::forward<keyT>(newKey);

        if (keyWithIV.length() != Twofish::MAX_KEYLENGTH * 3) {
            if(exists(newKey)) {
                std::ifstream keyFile(keyWithIV);

                keyWithIV.clear();
                keyFile >> keyWithIV;
                keyFile.close();
            }
            else if (!exists(newKey)) {
                auto keyFilePath(
                    static_cast<path>(currentPath())
                );

                keyFilePath /= keyWithIV;
                std::ifstream keyFile(keyFilePath.generic_string());

                keyWithIV.clear();
                keyFile >> keyWithIV;
                keyFile.close();
            }
            else {
                std::cerr << "Invalid key is specified..." << std::endl;
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

    inline bool willEncrypt() const noexcept {
        return m_willEncrypt;
    }

    inline void willEncrypt(bool trueOrFalse) noexcept {
        m_willEncrypt = trueOrFalse;
    }
};


#endif
