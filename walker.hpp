#ifndef WALKER_HPP
#define WALKER_HPP

#include <iostream>
#include "cryptographer.hpp"
#include <memory>
#include <type_traits>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

class Walker {
private:
    // Where we going to walk
    path m_walkThrough;
    // With we whom we're going to walk
    std::shared_ptr<Cryptographer> m_walkWith;

public:
    
    //Needs some consideration, but for now, i think it's ok;)
    template<typename pathT, typename walkWithT,
             typename = std::enable_if<
                            !std::is_base_of<
                                Walker,
                                std::decay_t<pathT>
                            >::value &&
                            std::is_constructible<
                                Cryptographer,
                                walkWithT
                            >::value

             >
    >
    explicit inline Walker(pathT&& walkTo, walkWithT&& walkWith) noexcept:
                            m_walkThrough(std::forward<pathT>(walkTo)),
                            m_walkWith(std::forward<walkWithT>(walkWith)) {
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

    void walk(bool recursively);
};
#endif