#include "block_cipher.hpp"

Block_Cipher::~Block_Cipher() {
    if(m_byteIV) {
        for(auto iii(0); iii < m_blockSize; ++iii)
            m_byteIV[iii] = 0;

        delete[] m_byteIV;
        m_byteIV = nullptr;
    }
}
