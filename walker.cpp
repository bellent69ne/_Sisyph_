/**************Soon will come***************/
#include "walker.hpp"

void Walker::walk() {
    current_path(m_walkThrough);

    auto walkingThrough(static_cast<directory_iterator>(m_walkThrough));

    while(walkingThrough != directory_iterator()) {
        if(!is_directory(*walkingThrough)) {
            std::cout << "Encrypting....." <<*walkingThrough << '\n';
            m_walkWith->decrypt(*walkingThrough);
            ++walkingThrough;
        }
        else
            ++walkingThrough;
    }
}
