#ifndef BLOCKCIPHER_HPP
#define BLOCKCIPHER_HPP

#include <string>
#include "shredder.hpp"
#include <memory>
#include "secblock.h"
using CryptoPP::SecByteBlock;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

class BlockCipher {
protected:
	fs::path m_currentPath;
	std::string m_encKey;
	std::string m_encIV;
	SecByteBlock m_byteKey;
	std::unique_ptr<byte[]> m_byteIV;
	bool m_willEncrypt;
	Shredder m_shredder;

public:

	//BlockCipher();

	template<typename pathT,
			 typename = std::enable_if_t<
			 				!std::is_base_of<
			 					BlockCipher,
			 					std::decay_t<pathT>
			 				>::value && 
			 				std::is_constructible<
			 					fs::path,
			 					std::decay_t<pathT>
			 				>::value
			 			>
	>
	explicit inline BlockCipher(pathT&& fullPath) noexcept:
							m_currentPath(std::forward<pathT>(fullPath)),
							m_encKey(""),
							m_encIV(""),
							m_willEncrypt(true) {
	}

	virtual void encrypt() = 0;

	template<typename pathT,
			 typename = std::enable_if_t<
			 				std::is_assignable<
			 					fs::path,
			 					std::decay_t<pathT>
			 				>::value
			 			>
	>
	void encrypt(pathT&& fullPath) {
		m_currentPath = std::forward<pathT>(fullPath);

		encrypt();
	}

	virtual void decrypt() = 0;

	template<typename pathT,
			 typename = std::enable_if_t<
			 				std::is_assignable<
			 					fs::path,
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

	inline virtual int keyLength() noexcept = 0;

	template<typename pathT,
			 typename = std::enable_if_t<
			 				std::is_assignable<
			 					fs::path,
			 					std::decay_t<pathT>
			 				>::value
			 			>
	>
	inline void setPath(pathT&& newPath) noexcept {
		m_currentPath = std::forward<pathT>(newPath);
	}

	virtual void generatekey() = 0;
	virtual void generateIV() = 0;

	inline auto getKey() noexcept {
		return m_encKey;
	}

	inline auto getIV() noexcept {
		return m_encIV;
	}

	template<typename keyT, 
			 typename = std::enable_if_t<
			 				std::is_assignable<
			 					std::string,
			 					std::decay_t<keyT>
			 				>::value
	>
	virtual void setKey(keyT&& newKey) = 0;

	template<typename ivT,
			 typename = std::enable_if_t<
			 				std::is_assignable<
			 					std::string,
			 					std::decay_t<ivT>
			 				>::value
			 			>
	>

	inline auto willEncrypt() const noexcept {
		return m_willEncrypt;
	}

	inline void willEncrypt(bool trueOrFalse) noexcept {
		m_willEncrypt = trueOrFalse;
	}

	BlockCipher() = default;

	virtual ~BlockCipher() = default;
};

#endif