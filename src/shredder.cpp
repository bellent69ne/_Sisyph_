#include "shredder.hpp"


void Shredder::shredFile() {
    if (!writeRandomData()) {
        return;
    }

    auto newPath(static_cast<fs::path>(randomRename()));
    boost::system::error_code ec;
    fs::remove(newPath, ec);
    
    if (ec) {
        std::cerr << "Failed removing " << newPath.generic_string() 
                  << ec.message() << std::endl;
    }
}