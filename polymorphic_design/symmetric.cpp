#include "symmetric.hpp"

void Symmetric::encrypt(const filesystem::path& fullPath) {
    // copying fullPath into m_currentPath
    m_currentPath = fullPath;
    // then encrypting that path
    encrypt();
}

void Symmetric::encrypt(filesystem::path&& fullPath) {
    // moving fullPath into m_currentPath
    m_currentPath = std::move(fullPath);
    // then encrypting that path
    encrypt();
}

void Symmetric::decrypt(const filesystem::path& fullPath) {
    // copying fullPath into m_currentPath
    m_currentPath = fullPath;
    // then decrypting that path
    decrypt();
}

void Symmetric::decrypt(filesystem::path&& fullPath) {
    // moving fullPath into m_currentPath
    m_currentPath = std::move(fullPath);
    // then decrypting that path
    decrypt();
}
