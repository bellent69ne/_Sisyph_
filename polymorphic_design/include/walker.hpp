#ifndef WALKER_HPP
#define WALKER_HPP

#include "symmetric.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <type_traits>
#include <array>
#include <boost/filesystem.hpp>

#include "twofish.hpp"
#include "rc6.hpp"


using namespace boost::filesystem;

/* Walker is like a skywalker from star wars;)
   Actually, this is just a guy, which walks over filesystem
   Imagine you want to rob a bank. Walker is the guy who knows
   everything about that bank, possibly even works there. He knows
   where are all cameras, knows everyone in security, knows how to walk,
   where to walk, and more importantly, he knows how to do all this stuff
   such as helping us to rob a bank and stay unknown
*/
class Walker {
private:
    // Where we going to walk
    path m_walkThrough;
    /* With whom we're going to walk. This guy m_walkWith
       is the guy who does all dirty work such as encrypting
       sensitive data like bank accounts, corporative and commercial
       secret data. It is symmetric, meaning that it can perform
       any symmetric cryptographic algorithm. In the future here will
       be another guy, who will be responsible for assymetric operations
    */
    std::unique_ptr<sisyph::Symmetric> m_walkWith;
    // Command line arguments
    std::vector<std::string> m_cmdArgs;
    // cmd argument location of path(index)
    int m_pathStarts;
    // compiler warning for uniform initialization
    //std::array<std::string, 2> m_supportedAlgorithms{"TWOFISH", "RC6"};
    /*std::array<std::string, 7> m_sysDirectories
        {"usr", "sbin", "proc", "sys", "var", "lib", "tmp"};*/
public:

    /*As always, perfect forwarding constructor;)
      Better refer to the appropriate documentation.
    */
    template<typename pathT,
        typename = std::enable_if_t<
            std::is_constructible<
                filesystem::path,
                std::decay_t<pathT>
            >::value
        >
    >   // Initialize all this stuff
    explicit inline Walker(pathT&& walkTo) noexcept:
            m_walkThrough(std::forward<pathT>(walkTo)),
            m_walkWith(std::make_unique<sisyph::RC6>()),
            m_pathStarts(0) {
            //m_supportedAlgorithms() {
    }

    // get the current walking location in the filesystem
    inline auto walks() const noexcept {
        return m_walkThrough.generic_wstring();
    }

    // perfect forwarding setter for the walking location
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

    /* This method will walk through all provided locations in the filesystem,
       and do the desired operation with that files
    */
    void walk();

    /* Will return true if we're walking in recursive mode(through all
       directories and it's subdirectories etc.)
     */
    inline auto isRecursive() {
        auto recursive(false);
        // Recursive option comes after the program name
        m_cmdArgs.at(1) == "-r"
            ? recursive = true
            : recursive = false;

        return recursive;
    }

    /*
        Checks all command line arguments provided, then determines
        appropriate operations to perform, values, algorithm and etc.
    */
    template<typename T, typename cmdArgs,
        typename = std::enable_if<
            std::is_integral<std::decay_t<T>>::value &&
            std::is_assignable<std::string,
                std::decay_t<cmdArgs>
            >::value
        >
    >
    void setCmdArgs(T&& paramsNum, cmdArgs params) {
        // Assign that container of all cmd arguments
        m_cmdArgs.assign(params, params + paramsNum);

        // Minimal number of cmd argumnets(params)
        // compiler warns about this
        const auto minNumbOfParams(4);

        /* Determine type of the operation. For now, only encryption
           and decryption works. In future, this list will be a lot bigger.
           Probably, I need to change this cmd interface.
        */
        auto operation([](auto&& option) {
            // if encryption takes place, then return true.
            // We assume that our program is targetted for encryption;)
            if (option == "--enc")
                return true;

            // if decryption takes place, then return false
            else if (option == "--dec")
                return false;

            // if it's neither encryption nor decryption,
            // better look at the usage message
            std::cerr << "Usage: ./sisyph -r(optional) "
                         "--enc ALGORITHM --keyGen /path"
                      << std::endl;
            exit(-1);
        });

        /*  Determines whether we're gonna generate encryption key.
            I know that software like gpg or openssl requires password,
            and I know that that's a lot better approach than providing
            encryption key. But for now, we will be using encryption key
            instead of password.
        */
        auto keyGen([](std::string& keyOption) {
            // if we wanna generate encryption  key, return true
            if (keyOption == "--keyGen")
                return true;

            // Otherwise, return false
            else
                return false;
        });

        // Check cmd option for supported algorithms
        /*auto supportedAlgorithm([&m_supportedAlgorithms = m_supportedAlgorithms]
            (std::string& algorithmName) {
            for(auto& each: m_supportedAlgorithms)
                if(each == algorithmName)
                    return true;

            return false;
        });*/

        try {
            /*
                Eventually somewhere in the command line options,
                will be provided paths to the files, or filenames.
                filePathToStart is simply index of the first file
                in command line arguments.
            */
            auto filePathsStart(0);

            /*  ./sisyph -r --enc ALGORITHM --keyGen filenames...
                -r here in the interface is optional, which means recursive.
                So if we want to walk recursively, then filePathsStart is equal
                to 5, otherwise it's equal to 4(without -r option)
            */
            isRecursive() ? filePathsStart = 6 : filePathsStart = 5;
            // copy that to our class private field m_pathStarts;
            m_pathStarts = filePathsStart;

            // Now just determine the cryptographic algorithm
            // if it's Twofish, allocate sisyph::Twofish cryptographic object
            if(m_cmdArgs.at(filePathsStart - 3) == "TWOFISH")
                m_walkWith.reset(new sisyph::Twofish(m_cmdArgs.at(filePathsStart - 2)));
            // if it's RC6, then allocate sisyph::RC6 cryptographic object
            else if(m_cmdArgs.at(filePathsStart - 3) == "RC6")
                m_walkWith.reset(new sisyph::RC6(m_cmdArgs.at(filePathsStart - 2)));
            // if nothing of these, then show usage message and exit
            else
                usage();

            // Set either we're gonna encrypt or not(--enc means encrypt)
            m_walkWith->willEncrypt(
                operation(m_cmdArgs.at(filePathsStart - 4))
            );


            // if we wanna generate encryption key, then generate it
            // Otherwise forward that key, and set it
            keyGen(m_cmdArgs.at(filePathsStart - 1)) ?
                m_walkWith->generateKey() :
                m_walkWith->setKey(m_cmdArgs.at(filePathsStart - 1));
        }
        catch(const std::exception &e) {
            usage();
        }
    }


    inline void usage() {
        std::cerr << "Usage: ./sisyph -r(optional)"
                     " --enc ALGORITHM --keyGen /path"
                  << std::endl;
        exit(-1);
    }
};


#endif
