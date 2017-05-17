/**************Soon will come***************/
#include "walker.hpp"
using namespace boost::filesystem;
void Walker::walk(bool recursively) {
    if(m_walkWith->getKey().empty() || 
       m_walkWith->getIV().empty()) {
        std::cerr << "Key or iv isn't specified..." << '\n';
        exit(1);
    }

    current_path(m_walkThrough);

    auto whatRWeDoing(static_cast<std::string>(""));
    m_walkWith->goingToEncrypt() ? whatRWeDoing = "Encrypting------------->"
                               : whatRWeDoing = "Decrypting------------->";

    auto walkNow([m_walkWith = m_walkWith, &whatRWeDoing]
            (auto walkingThrough, auto recursive_directory_iterator) {
        while(walkingThrough != recursive_directory_iterator) {
            if(!is_directory(*walkingThrough)) {
                std::cout << whatRWeDoing <<*walkingThrough << '\n';
                m_walkWith->goingToEncrypt() ? m_walkWith->encrypt(*walkingThrough++)
                                               : m_walkWith->decrypt(*walkingThrough++);
            }
            else
                ++walkingThrough;
        }
    });

    
    recursively ? walkNow(recursive_directory_iterator(m_walkThrough), 
                                    recursive_directory_iterator())
                : walkNow(directory_iterator(m_walkThrough), 
                                    directory_iterator());
}
