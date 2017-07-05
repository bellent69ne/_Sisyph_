#ifndef Symmetric_HPP
#define Symmetric_HPP

#include "namespace_sisyph.hpp"
#include <string>
#include "osrng.h"
#include "hex.h"
#include <boost/filesystem.hpp>
using namespace boost;

/* Base class for All symmetric cryptographic
   algorithms
*/
class sisyph::Symmetric {
protected:
    // current path whic we are processing
    filesystem::path m_currentPath;
    // encoded in hex encryption key
    std::string m_encKey;
    // either we're gonna encrypt or not
    bool m_willEncrypt;

    /* Universal encryption key and initialization vector(IV)
       generation method;
       byteSequence is byte container either byte array or SecByteBlock,
       sizeOfContainer is consequently the size of the passed byte container,
       and decodeHere is the location where hex encoded data
       have to be stored;
    */
    template<typename byteSequence>
    void generate(byteSequence& byteContainer, const char sizeOfContainer,
                  std::string& encodeHere) {
        // Generate key or IV in bytes
        CryptoPP::AutoSeededRandomPool prng;
        prng.GenerateBlock(byteContainer, sizeOfContainer);

        // Clear encodedHere to store there hex encoded key or IV representation
        encodeHere.clear();
        // Encode byte container(key or IV) into hex
        CryptoPP::StringSource ssKey(byteContainer, sizeOfContainer, true,
            new CryptoPP::HexEncoder(    // Hex encoder object
                new CryptoPP::StringSink(encodeHere)    // Where it encodes that
            )                                           // byte container
        );
    }

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
    explicit Symmetric(pathT&& fullPath) noexcept:
            m_currentPath(std::forward<pathT>(fullPath)),
            m_encKey(""),
            m_willEncrypt(true) {
    }

    // We just need default compiler generated contructor
    Symmetric() = default;

    // main implementation of file encryption
    virtual void encrypt() = 0;
    // perfect forwards fullPath to m_currentPath, then encrypts that path
    template<typename pathT,
        typename = std::enable_if_t<
            std::is_assignable<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    void encrypt(pathT&& fullPath) {
        m_currentPath = std::forward<pathT>(fullPath);

        encrypt();
    }

    // main implementation of file decryption
    virtual void decrypt() = 0;
    // perfect forwards fullPath to m_currentPath, then decrypts that path
    template<typename pathT,
        typename = std::enable_if_t<
            std::is_assignable<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    void decrypt(pathT&& fullPath) {
        m_currentPath = std::forward<pathT>(fullPath);

        decrypt();
    }

    // get length of key
    virtual char keyLength() noexcept = 0;

    // get current path on which we operate
    virtual filesystem::path currentPath() noexcept;

    // perfect forwards newPath to m_currentPath
    template<typename pathT,
        typename = std::enable_if_t<
            std::is_assignable<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >
    inline void currentPath(pathT&& newPath) noexcept {
        m_currentPath = std::forward<pathT>(newPath);
    }

    // implementation of key generation
    virtual void generateKey() = 0;

    // getter for m_encKey
    virtual std::string getKey() noexcept;

    // setter for m_encKey(copying newKey)
    virtual void setKey(std::string& newKey) = 0;

    //virtual void setKey(std::string&& newKey) = 0;

    // returns either will it encrypt or not
    virtual bool willEncrypt() const noexcept;

    // sets either will it encrypt or not
    virtual void willEncrypt(const bool trueOrFalse) noexcept;

    // compiler generated virtual default contructor
    virtual ~Symmetric() = default;

};

#endif
