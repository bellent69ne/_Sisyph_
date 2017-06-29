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

    constexpr auto permittedMemory(1024 * 1024 * 10);

    auto numOfBranches(0);
    auto overwrite([&numOfBranches = numOfBranches,
                    &permittedMemory = permittedMemory,
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

        auto processOverwrite([&writeAndFlush = writeAndFlush,
                               &generate = generate,
                               &randomSeeder = randomSeeder,
                               &buffer = buffer](long long startingPoint) {
            for(auto& bufferElement: buffer)
                bufferElement = generate(randomSeeder);

            writeAndFlush(startingPoint);

            for(auto& bufferElement: buffer)
                bufferElement = 0;

            writeAndFlush(startingPoint);
        });

        // if we're gonna overwrite via blocks of data
        if(viaBlocks) {
            thread_local auto startingPoint(permittedMemory * index);

            while(startingPoint < fileSize) {
                buffer.resize(permittedMemory);

                processOverwrite(startingPoint);

                startingPoint += permittedMemory * numOfBranches;
            }
        }

        else {
            buffer.resize(fileSize);
            processOverwrite(0);
        }

    });

    if(fileSize <= permittedMemory)
        overwrite(false, 0);

    else {
        std::vector<std::future<void>> overwritingTasks;
        auto taskIndex(0);
        while(taskIndex <= 3) {
            overwritingTasks.push_back(std::async(overwrite, true, taskIndex));
            ++taskIndex;
        }

        for(auto& eachTask: overwritingTasks)
            eachTask.wait();
    }

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
