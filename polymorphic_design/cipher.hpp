#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <string>
#include <boost/filesystem.hpp>
using namespace boost;

class Cipher {
protected:
    filesystem::path m_currentPath;
    std::string m_encKey;
    bool m_willEncrypt;

    //template<typename pathT,
    //    typename = std::enable_if_t<
    //        !std::is_base_of<
    //            Cipher,
    //            std::decay_t<pathT>
    //        >::value &&
    //        std::is_constructible<
    //            filesystem::path,
    //            std::decay_t<pathT>
    //        >::value
    //    >
    //>
public:
    explicit inline Cipher(const filesystem::path& fullPath) noexcept:
            m_currentPath(fullPath),
            m_encKey(""),
            m_willEncrypt(true) {
    }
    explicit inline Cipher(filesystem::path&& fullPath) noexcept:
            m_currentPath(std::move(fullPath)),
            m_encKey(""),
            m_willEncrypt(true) {
    }

    Cipher() = default;

    virtual void encrypt() = 0;

    virtual void encrypt(const filesystem::path& fullPath) = 0;

    virtual void encrypt(filesystem::path&& fullPath) = 0;

    virtual void decrypt() = 0;

    virtual void decrypt(const filesystem::path& fullPath) = 0;

    virtual void decrypt(filesystem::path&& fullPath) = 0;

    inline virtual filesystem::path currentPath() noexcept = 0;

    inline virtual int keyLength() noexcept = 0;

    inline virtual void setPath(const filesystem::path& newPath) noexcept = 0;

    inline virtual void setPath(filesystem::path&& newPath) noexcept = 0;

    virtual void generateKey() = 0;

    inline virtual std::string getKey() noexcept {
        return m_encKey;
    }

    virtual void setKey(const std::string& newKey) = 0;

    //virtual void setKey(std::string&& newKey) = 0;

    inline virtual bool willEncrypt() const noexcept {
        return m_willEncrypt;
    }

    inline virtual void willEncrypt(bool trueOrFalse) noexcept {
        m_willEncrypt = trueOrFalse;
    }

    virtual ~Cipher() = default;

};

#endif
