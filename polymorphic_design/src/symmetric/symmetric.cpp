#include "symmetric.hpp"

filesystem::path sisyph::Symmetric::currentPath() noexcept {
    return m_currentPath;
}

std::string sisyph::Symmetric::getKey() noexcept {
    return m_encKey;
}

bool sisyph::Symmetric::willEncrypt() const noexcept {
    return m_willEncrypt;
}

void sisyph::Symmetric::willEncrypt(bool trueOrFalse) noexcept {
    m_willEncrypt = trueOrFalse;
}
