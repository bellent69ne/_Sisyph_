#ifndef Symmetric_HPP
#define Symmetric_HPP

#include <string>
#include <boost/filesystem.hpp>
using namespace boost;

/* Base class for All symmetric cryptographic
   algorithms
*/
class Symmetric {
protected:
    // current path whic we are processing
    filesystem::path m_currentPath;
    // encoded in hex encryption key
    std::string m_encKey;
    // either we're gonna encrypt or not
    bool m_willEncrypt;

public:
    /*  Perfect forwarding contructor. Used to construct our fields either
        by moving objects into them, or by copying objects into them.
        Replaces overloading for moving and copying objects into fields
        (One universal function for both cases).
    */
    template<typename pathT,
        typename = std::enable_if_t<    // A little bit of template
            !std::is_base_of<           // metaprogramming
                Symmetric,
                std::decay_t<pathT>     // I think it's straightforward
            >::value &&
            std::is_constructible<      // If it's not obvious,
                filesystem::path,       // well, then obtain some
                std::decay_t<pathT>     // metaprogramming skills
            >::value
        >
    >
    explicit inline Symmetric(pathT&& fullPath) noexcept:
            m_currentPath(std::forward<pathT>(fullPath)),
            m_encKey(""),
            m_willEncrypt(true) {
    }

    // We just need default compiler generated contructor
    Symmetric() = default;

    // main implementation of file encryption
    virtual void encrypt() = 0;

    // passing path for encryption, and then encrypting that path(copying path)
    virtual void encrypt(const filesystem::path& fullPath);

    // passing path for encryption, and then encrypting that path(moveing path)
    virtual void encrypt(filesystem::path&& fullPath);

    // main implementation of file decryption
    virtual void decrypt() = 0;

    // passing path for decryption, and then decrypting that path(copying path)
    virtual void decrypt(const filesystem::path& fullPath);

    // passing path for decryption. and then decrypting that path(moving path)
    virtual void decrypt(filesystem::path&& fullPath);

    // get current path on which we operate
    inline virtual filesystem::path currentPath() noexcept {
        return m_currentPath;
    }

    // get length of key
    inline virtual int keyLength() noexcept = 0;

    // setter for m_currentPath(copying path)
    inline virtual void currentPath(const filesystem::path& newPath) noexcept {
        m_currentPath = newPath;
    }

    // setter for m_currentPath(moving path)
    inline virtual void currentPath(filesystem::path&& newPath) noexcept {
        m_currentPath = std::move(newPath);
    }

    // implementation of key generation
    virtual void generateKey() = 0;

    // getter for m_encKey
    inline virtual std::string getKey() noexcept {
        return m_encKey;
    }

    // setter for m_encKey(copying newKey)
    virtual void setKey(const std::string& newKey) = 0;

    //virtual void setKey(std::string&& newKey) = 0;

    // returns either will it encrypt or not
    inline virtual bool willEncrypt() const noexcept {
        return m_willEncrypt;
    }

    // sets either will it encrypt or not
    inline virtual void willEncrypt(bool trueOrFalse) noexcept {
        m_willEncrypt = trueOrFalse;
    }

    // compiler generated virtual default contructor
    virtual ~Symmetric() = default;

};

#endif
