#include "shredder.hpp"
#include <future>

fs::path Shredder::randomRename() {
    constexpr auto maxRenameAttempts(10);
    auto attempts(0);

    auto newPath(static_cast<fs::path>(""));
    while(attempts < maxRenameAttempts) {
        newPath =
            static_cast<const fs::path>(
                m_fileToShred.parent_path() / fs::unique_path().generic_string()
            );

        if(!fs::exists(newPath)) {
            boost::system::error_code ec;
            fs::rename(m_fileToShred, newPath, ec);
            if(!ec)
                break;

            else if(ec) {
                std::cerr << "Failed renaming "
                          << fs::absolute(m_fileToShred)
                          << " to "
                          << fs::absolute(newPath)
                          << std::endl;
            }

        }
        ++attempts;
    }
    return newPath;
}

bool Shredder::writeRandomData() {
    boost::system::error_code ec;
    const auto fileSize(
        static_cast<long long>(
            fs::file_size(m_fileToShred, ec)
        )
    );

    if (ec) {
        std::cerr << "Failed determining the size of "
                  << fs::absolute(m_fileToShred)
                  << ec.message()
                  << std::endl;

        return false;
    }

    std::ofstream fout(
        m_fileToShred.generic_string(), std::ios::binary
    );
    if (!fout) {
        std::cerr << "Failed opening "
                  << fs::absolute(m_fileToShred)
                  << strerror(errno)
                  << std::endl;

        return false;
    }

    //const auto blockSize(1024);
    //std::vector<unsigned char> buffer(blockSize);

    auto numOfBranches(0);
    auto overwrite([&numOfBranches = numOfBranches,
                    &fileSize = fileSize, &fout = fout]
                    (bool viaBlocks, short index) {
        ++numOfBranches;

        std::random_device seed;
        std::mt19937 randomSeeder(seed());
        std::uniform_int_distribution<> generate(0, 127);

        thread_local std::vector<char> buffer;

        auto writeAndFlush([&fout = fout, &buffer = buffer]
            (long long startingPoint) {
            fout.seekp(startingPoint, std::ios::beg);
            fout.write((char *) buffer[0], buffer.size());
            fout.flush();
        });
        // if we're gonna overwrite via blocks of data
        if(viaBlocks) {
            const auto blockSize(1024);

            thread_local auto startingPoint(blockSize * index);

            while(startingPoint < fileSize) {
                buffer.resize(blockSize);

                for(auto& bufferElement: buffer)
                    bufferElement = generate(randomSeeder);

                writeAndFlush(startingPoint);

                for(auto& bufferElement: buffer)
                    bufferElement = 0;

                writeAndFlush(startingPoint);

                startingPoint += blockSize * numOfBranches;
            }
        }

        else {
            buffer.resize(fileSize);

            for(auto& bufferElement: buffer)
                bufferElement = generate(randomSeeder);

            writeAndFlush(0);

            for(auto& bufferElement: buffer)
                bufferElement = 0;

            writeAndFlush(0);
        }

    });

/*    auto fileItrLocation(static_cast<long long>(0));

    while (fileItrLocation < fileSize) {
      /*  auto iterations(5);

        // Overwrite file with ASCII 255 and ASCII 0
        while (iterations--) {
            const auto ch(
                static_cast<unsigned char>(
                    (iterations & 1) ? 255 : 0
                )
            );

            for (auto& bufferElement: buffer) {
                bufferElement = ch;
            }

            fout.seekp(fileItrLocation, std::ios::beg);
            fout.write((char*) &buffer[0], buffer.size());
            fout.flush();
        }

        std::random_device seed;
        std::mt19937 randomSeeder(seed());
        std::uniform_int_distribution<> generate(0, 127);

        for (auto& bufferElement: buffer) {
            bufferElement = generate(randomSeeder);
        }

        fout.seekp(fileItrLocation, std::ios::beg);
        fout.write((char*) &buffer[0], buffer.size());
        fout.flush();

        // Overwrite file with null chars
        for (auto& bufferElement: buffer) {
            bufferElement = 0;
        }

        fout.seekp(fileItrLocation, std::ios::beg);
        fout.write((char*) &buffer[0], buffer.size());
        fout.flush();

        // Move to the next block
        fileItrLocation += blockSize;
    }*/

    // Change file size to 0
    auto destroy(std::async(overwrite, true, 0));
    auto destroy1(std::async(overwrite, true, 1));
    auto destroy2(std::async(overwrite, true, 2));
    auto destroy3(std::async(overwrite, true, 3));
    destroy.wait();
    destroy1.wait();
    destroy2.wait();
    destroy3.wait();

    fout.close();
    fout.open(m_fileToShred.generic_string(), std::ios::binary);

    fout.close();

    return true;
}


void Shredder::shredFile() {
    if (!writeRandomData()) {
        return;
    }

    auto newPath(randomRename());

    boost::system::error_code ec;
    fs::remove(newPath, ec);

    if (ec) {
        std::cerr << "Failed removing "
                  << newPath.generic_string()
                  << ec.message()
                  << std::endl;
    }
}
