#ifndef BLOCK_CIPHER_HPP
#define BLOCK_CIPHER_HPP

#include "symmetric.hpp"
#include "secblock.h"
#include "filters.h"
#include "hex.h"
using namespace CryptoPP;

class Block_Cipher: public Symmetric {
protected:
    SecByteBlock m_byteKey;
    std::string m_encIV;
    byte *m_byteIV;
    char m_blockSize;
    char m_keyLength;

public:
    template<typename pathT,
        typename = std::enable_if_t<
            !std::is_base_of<
                BlockCipher,
                std::decay_t<pathT>
            >::value &&
            std::is_constructible<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    explicit Block_Cipher(pathT&& fullPath, char keyLength,
                          char blockSize):
            Symmetric(std::forward<pathT>(fullPath)),
            m_byteKey(keyLength),
            m_encIV(""),
            m_byteIV(new byte[blockSize]),
            m_blockSize(blockSize) {
    }

    virtual void generateIV() = 0;

    virtual inline std::string getIV() noexcept {
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


    inline virtual char keyLength() noexcept override {
        return m_keyLength;
    }

    virtual ~Block_Cipher() {
        if(m_byteIV) {
            for(auto iii(0); iii < m_blockSize; ++iii)
                m_byteIV[iii] = 0;

            delete[] m_byteIV;
            m_byteIV = nullptr;
        }
    }

};

#endif
