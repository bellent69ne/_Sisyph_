/**************Soon will come***************/
#include "walker.hpp"
namespace fs = boost::filesystem;

void Walker::walk(bool recursively) {
    if(m_walkWith->getKey().empty() || 
       m_walkWith->getIV().empty()) {
        std::cerr << "Key or iv isn't specified..." << '\n';
        exit(1);
    }

    findPath();
    std::cout << std::boolalpha << m_processAll << std::endl;

    std::cout << m_walkThrough.generic_string() << std::endl;

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

void Walker::findPath() {
    auto contains([&m_walkThrough = m_walkThrough](std::string&& pattern) {
        auto reallyContains(false);
        //std::string givenPath(m_walkThrough.generic_string());
        if(pattern.length() == 1)
            for(const auto& each: m_walkThrough.generic_string())
                if(each == pattern[0])
            //for(auto iii(0); iii < givenPath.length(); ++iii)
              //  if(givenPath[iii] == pattern)
            //for(auto iii(givenPath.cbegin()); iii != givenPath.cend(); ++iii)
              //  if(*iii == pattern)
                    reallyContains = true;

        else {
            auto isIn(static_cast<std::string>(""));

            for(const auto& each: pattern) 
                for(const auto& inPath: m_walkThrough.generic_string())
                    if(each == inPath)
                        isIn += each;

            isIn == pattern ? reallyContains = true
                            : reallyContains = false;
        }

        return reallyContains;
    });


    /*Shit.... Looks like a bad code, but now 
     *I don't know the better way to implement this
     */
    auto homeDir([&m_walkThrough = m_walkThrough] {
    //    if(isTrue)
            getenv("USER") == std::string("root") ? m_walkThrough = "/root"
                                : m_walkThrough = std::string("/home/") + getenv("USER");
    });

    auto currentPath([&m_walkThrough = m_walkThrough] {
//        if(isTrue) {
            m_walkThrough = "";
            m_walkThrough = fs::current_path();
        //}
    });

    auto outerPath([&m_walkThrough = m_walkThrough] {
        //if(isTrue) {
            auto subFromLast(0);
            for(auto iii(fs::current_path().generic_string().cend()); 
                        *iii != '/'; --iii)
                ++subFromLast;
            std::string theActualPath(current_path().generic_string().cbegin(),
                                    current_path().generic_string().cend() 
                                    - subFromLast);

            m_walkThrough = theActualPath;
        //}
    });

// . (*-strange behaviour)

    /*if(m_walkThrough.generic_string().length() == 1) {
        homeDir(contains("~"));
        //currentPath(contains("."));

        if(contains("*")) {
            m_processAll = true;
            m_walkThrough = fs::current_path();
        }
        else {
            //auto dirName(m_walkThrough);
            m_walkThrough = (fs::current_path() /= m_walkThrough);
            //m_walkThrough /= dirName;
        }
    }


    else if(m_walkThrough.generic_string().length() == 2) {
        homeDir(contains("~/"));
        //currentPath(contains("./"));

        outerPath(contains(".."));

        if(contains("/*")) {
            m_processAll = true;
            m_walkThrough = "/";
        }

    }

    else if(m_walkThrough.generic_string().length() == 3) {
        outerPath(contains("../"));

        if(contains("~/*")) {
            homeDir(contains("~/"));
            m_processAll = true;
        }

        else {
            //auto dirName(m_walkThrough);
            m_walkThrough = (fs::current_path() /= m_walkThrough);
        }
    }*/
    auto pathLength(m_walkThrough.generic_string().length());
    if(pathLength == 1 && contains("~"))
        homeDir();
    else if(pathLength == 1 && contains("."))
        currentPath();
    else if(pathLength == 1 && contains("*"))
        m_processAll = true;
    else if(pathLength == 2 && contains("~/"))
        homeDir();
    else if(pathLength == 2 && contains("./"))
        currentPath();
    else if(pathLength == 2 && contains("/*")) {
        m_processAll = true;
        m_walkThrough = "/";
    }

}