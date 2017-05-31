#include "shredder.hpp"


void Shredder::shredFile() {
    if (!writeRandomData()) {
        return;
    }

    auto newPath(randomRename());
    //fs::path newPath(randomRename(renamedPath));

    boost::system::error_code ec;
    fs::remove(newPath, ec);

    if (ec) {
        std::cerr << "Failed removing "
                  << newPath.generic_string()
                  << ec.message()
                  << std::endl;
    }
}
