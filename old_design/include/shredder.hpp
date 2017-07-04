#ifndef SHREDDER_HPP
#define SHREDDER_HPP

#include <memory>
#include <cstdlib>
#include <random>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


class Shredder {
private:
    fs::path m_fileToShred;
public:
    template<typename T,
        typename = std::enable_if<
            !std::is_base_of<
                Shredder,
                std::decay_t<T>
            >::value &&
                std::is_constructible<fs::path,
                std::decay_t<T>
            >::value
        >
    >
    explicit Shredder(T&& pathToFile):
                    m_fileToShred(std::forward<T>(pathToFile)) {
    }

    fs::path randomRename();

    bool writeRandomData();

    template<typename T,
        typename = std::enable_if<
            //std::is_pointer<T>::value &&
            std::is_assignable<fs::path,
                std::decay_t<T>
            >::value
        >
    >
    void shredFile(T&& newFile) {
        m_fileToShred = std::forward<T>(newFile);

        shredFile();
    }

    void shredFile();

    Shredder() = default;
};


#endif
