#ifndef CBCTWOFISH_HPP
#define CBCTWOFISH_HPP

#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
using namespace boost;

#include "symmetric.hpp"

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


class CBCTwofish: public Symmetric {
private:
    std::string m_encIV;
    SecByteBlock m_byteKey;
    byte m_byteIV[Twofish::BLOCKSIZE];
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
                filesystem::path,
                std::decay_t<T>
            >::value
        >
    >
    explicit inline CBCTwofish(T&& fullPath) noexcept:
            Symmetric(std::forward<T>(fullPath)),
            m_encIV(""),
            m_shredder(),
            m_loggedFiles("loggedFiles.dat") {
    }


    virtual void encrypt() override;

    virtual void decrypt() override;


    inline virtual int keyLength() noexcept override {
        return Twofish::MAX_KEYLENGTH;
    }

    virtual void generateKey() override;

    void generateIV();

    inline virtual std::string getKey() noexcept override {
        return m_encKey + m_encIV;
    }

    inline auto getIV() noexcept{
        return m_encIV;
    }

    virtual void setKey(const std::string& newKey) override;

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

};


#endif
