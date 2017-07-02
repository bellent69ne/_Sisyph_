#ifndef WALKER_HPP
#define WALKER_HPP

#include "cipher.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <type_traits>
#include <array>
#include <boost/filesystem.hpp>

//#include "cbctwofish.hpp"


using namespace boost::filesystem;


class Walker {
private:
    // Where we going to walk
    path m_walkThrough;
    // With whom we're going to walk
    std::shared_ptr<Cipher> m_walkWith;
    std::vector<std::string> m_cmdArgs;
    int m_pathStarts;
    std::array<std::string, 7> m_sysDirectories
        {"usr", "sbin", "proc", "sys", "var", "lib", "tmp"};
public:
    //Needs some consideration, but for now, i think it's ok;)
    template<typename pathT, typename walkWithT,
        typename = std::enable_if<
            !std::is_base_of<
                Walker,
                std::decay_t<pathT>
            >::value && std::is_constructible<
                Cipher,
                walkWithT
            >::value

        >
    >
    explicit inline Walker(pathT&& walkTo, walkWithT walkWith) noexcept:
            m_walkThrough(std::forward<pathT>(walkTo)),
            m_walkWith(walkWith),
            m_pathStarts(0) {
    }

    inline auto walks() const noexcept {
        return m_walkThrough.generic_wstring();
    }

    template <typename walkToT,
        typename = std::enable_if<
            std::is_assignable<
                path,
                std::decay_t<walkToT>
            >::value
        >
    >
    void walks(walkToT&& walkTo) noexcept {
        m_walkThrough = std::forward<walkToT>(walkTo);
    }


    /*inline auto walksWith() const noexcept {
        return m_walkWith;
    }*/

    /*For now, I don't know how should I set Cryptographer.
     *Cryptographer's default assignments won't do the job well,
     *because Cryptographer holds array of bytes, which as I know
     *can be copied only by each element.
     *So for now, I'll omit this. We cannot set Cryptographer.
     */

    void walk();

    inline auto isRecursive() {
        auto recursive(false);
        m_cmdArgs.at(1) == "-r"
            ? recursive = true
            : recursive = false;

        return recursive;
    }

    template<typename T, typename cmdArgs,
        typename = std::enable_if<
            std::is_integral<std::decay_t<T>>::value &&
            std::is_assignable<std::string,
                std::decay_t<cmdArgs>
            >::value
        >
    >
    void setCmdArgs(T&& paramsNum, cmdArgs params) {
        m_cmdArgs.assign(params, params + paramsNum);

        const auto minNumbOfParams(4);

        auto operation([](std::string& option) {
            if (option == "-e" || option == "--encrypt") {
                return true;
            }
            else if (option == "-d" || option == "--decrypt") {
                return false;
            }

            std::cerr << "Usage: ./sisyph -r(optional) "
                         "--encrypt --keyGen /path"
                      << std::endl;
            exit(-1);
        });

        auto keyGen([](std::string& keyOption) {
            if (keyOption == "--keyGen") {
                return true;
            }
            else {
                return false;
            }
        });

        try {
            isRecursive()
                ? m_walkWith->willEncrypt(operation(m_cmdArgs.at(2)))
                : m_walkWith->willEncrypt(operation(m_cmdArgs.at(1)));



                if (isRecursive()) {
                    if (keyGen(m_cmdArgs.at(3))) {
                        m_walkWith->generateKey();
                        //m_walkWith->generateIV();
                    }
                    else {
                        m_walkWith->setKey(m_cmdArgs.at(3));
                    }
                }

                else {
                    if (keyGen(m_cmdArgs.at(2))) {
                        m_walkWith->generateKey();
                        //m_walkWith->generateIV();
                    }
                    else {
                        m_walkWith->setKey(m_cmdArgs.at(2));
                    }
                }

                isRecursive() ? m_pathStarts = 4 : m_pathStarts = 3;
            }
            catch(const std::exception &e) {
                usage();
            }
    }


    inline void usage() {
        std::cerr << "Usage: ./sisyph -r(optional)"
                     " --encrypt --keyGen /path"
                  << std::endl;
        exit(-1);
    }
};


#endif
