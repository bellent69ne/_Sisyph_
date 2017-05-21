/**************Soon will come***************/
#include "walker.hpp"
namespace fs = boost::filesystem;

void Walker::walk() {
    if(m_walkWith->getKey().empty() || 
       m_walkWith->getIV().empty()) {
        std::cerr << "Key or iv isn't specified..." << '\n';
        exit(1);
    }

    std::cout << std::boolalpha << isRecursive() << std::endl;

    std::cout << m_walkWith->willEncrypt() << std::endl;

    std::cout << m_walkWith->getKey() + m_walkWith->getIV() << std::endl;

    /*auto whatRWeDoing(static_cast<std::string>(""));
    m_walkWith->goingToEncrypt() ? whatRWeDoing = "Encrypting------------->"
                               : whatRWeDoing = "Decrypting------------->";

    if(fs::is_regular_file(m_walkThrough)) {
        std::cout << whatRWeDoing << m_walkThrough << '\n';
        m_walkWith->goingToEncrypt() ? m_walkWith->encrypt(m_walkThrough)
                                     : m_walkWith->decrypt(m_walkThrough);
    }

    /*else if(fs::is_directory(m_walkThrough)) {
        auto walkNow([m_walkWith = m_walkWith, &whatRWeDoing]
                (auto walkingThrough, auto directory_iterator) {
            while(walkingThrough != directory_iterator) {
                if(fs::is_regular_file(*walkingThrough)) {
                    std::cout << whatRWeDoing <<*walkingThrough << '\n';
                    m_walkWith->goingToEncrypt() ? m_walkWith->encrypt(*walkingThrough++)
                                               : m_walkWith->decrypt(*walkingThrough++);
                }
                else
                    ++walkingThrough;
            }
        });
    
        /*recursively ? walkNow(fs::recursive_directory_iterator(m_walkThrough), 
                                        fs::recursive_directory_iterator())
                    : walkNow(fs::directory_iterator(m_walkThrough), 
                                        fs::directory_iterator());
    }*/
}
