#ifndef CBCTWOFISH_HPP
#define CBCTWOFISH_HPP

#include <fstream>
#include "block_cipher.hpp"
#include "twofish.h"
#include "files.h"
#include "modes.h"
#include "shredder.hpp"


class CBCTwofish: public Block_Cipher {
private:
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
            Block_Cipher(std::forward<T>(fullPath), Twofish::MAX_KEYLENGTH,
                         Twofish::BLOCKSIZE),
            m_shredder(),
            m_loggedFiles("loggedFiles.dat") {
    }


    virtual void encrypt() override;

    virtual void decrypt() override;

    virtual void generateKey() override;

    virtual void generateIV() override;

    inline virtual std::string getKey() noexcept override {
        return m_encKey + m_encIV;
    }

    virtual void setKey(const std::string& newKey) override;

};


#endif
