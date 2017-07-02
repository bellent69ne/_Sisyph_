#include "walker.hpp"

namespace fs = boost::filesystem;


void Walker::walk() {
    if (m_walkWith->getKey().empty()) //|| m_walkWith->getIV().empty()) {
        std::cerr << "Key or iv isn't specified..." << '\n';
        exit(1);
    }

    auto whatRWeDoing(static_cast<std::string>(""));
    m_walkWith->willEncrypt() ?
            whatRWeDoing = "Encrypting ---------------------------> " :
            whatRWeDoing = "Decrypting ---------------------------> ";

    try {
        auto pathToArg(m_cmdArgs.cbegin() + m_pathStarts);

        while (pathToArg != m_cmdArgs.cend()) {
            fs::exists(*pathToArg) ?
                    m_walkThrough = *pathToArg :
                    m_walkThrough = fs::current_path() / *pathToArg;

            /*Checking for system directories(for example "/proc")*/
            auto inSysDir([&m_sysDirectories = m_sysDirectories](auto&& somePath) {
                auto afterSlash(somePath.begin());
                ++afterSlash;

                for(auto &sysDir: m_sysDirectories)
                    if(*afterSlash == sysDir)
                        return true;

                return false;
            });

            if (fs::is_regular_file(m_walkThrough)) {
                std::cout << whatRWeDoing
                          << m_walkThrough.generic_string() << '\n';
                m_walkWith->willEncrypt() ?
                        m_walkWith->encrypt(m_walkThrough) :
                        m_walkWith->decrypt(m_walkThrough);
            }
            else if (fs::is_directory(m_walkThrough) && isRecursive()) {
                auto recursiveItr(
                    static_cast<fs::recursive_directory_iterator>(m_walkThrough)
                );

                while (recursiveItr != fs::recursive_directory_iterator()) {
                    if (fs::is_regular_file(*recursiveItr)) {
                        std::cout << whatRWeDoing << *recursiveItr << '\n';

                        m_walkWith->willEncrypt() ?
                                m_walkWith->encrypt(*recursiveItr++) :
                                m_walkWith->decrypt(*recursiveItr++);
                    }

                    else
                        ++recursiveItr;
                }
            }
            ++pathToArg;
        }
    } catch(const fs::filesystem_error& e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
