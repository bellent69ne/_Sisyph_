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
            activityT twofish;
            twofish.SetKeyWithIV(
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
                new StreamTransformationFilter(twofish,
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
    Twofish();

    template<typename T,
        typename = std::enable_if_t<
            !std::is_base_of<
                sisyph::Twofish,
                std::decay_t<T>
            >::value && std::is_constructible<
                filesystem::path,
                std::decay_t<T>
            >::value
        >
    >
    explicit inline Twofish(T&& fullPath) noexcept:
            BlockCipher(std::forward<T>(fullPath),
                        CryptoPP::Twofish::MAX_KEYLENGTH,
                        CryptoPP::Twofish::BLOCKSIZE),
            m_shredder(),
            m_loggedFiles("loggedFiles.dat") {
    }


    virtual void encrypt() override;

    virtual void decrypt() override;

    virtual void generateKey() override;

    virtual void generateIV() override;

     virtual std::string getKey() noexcept override {
        return m_encKey + m_encIV;
    }

    virtual void setKey(const std::string& newKey) override;
};


#endif
