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
    // ec means error_code
    boost::system::error_code ec;
    // determining the actual size of file
    const auto fileSize(
        static_cast<long long>(
            fs::file_size(m_fileToShred, ec)
        )
    );
    /* if error occured during determining the size of file\
     * report that
     */
    if (ec) {
        std::cerr << "Failed determining the size of "
                  << fs::absolute(m_fileToShred)
                  << ec.message()
                  << std::endl;

        return false;
    }
    // Open the file for writing
    std::ofstream fout(
        m_fileToShred.generic_string(), std::ios::binary
    );
    /* if couldn't open file for writing
     * report that
     */
    if (!fout) {
        std::cerr << "Failed opening "
                  << fs::absolute(m_fileToShred)
                  << strerror(errno)
                  << std::endl;

        return false;
    }
    // permitted memory for processing
    constexpr auto permittedMemory(1024 * 1024 * 10);
    // number of branches meaning the total number of tasks running
    auto numOfBranches(0);
    // Actual implementation of file overwriting
    auto overwrite([&numOfBranches = numOfBranches,
                    &permittedMemory = permittedMemory,
                    &fileSize = fileSize, &fout = fout]
                    (bool viaBlocks, short index) {
        // when task is launched, increment the total number of tasks
        ++numOfBranches;
        // modern style random number generation
        std::random_device seed;
        std::mt19937 randomSeeder(seed());
        std::uniform_int_distribution<> generate(0, 127);
        // generate(randomSeeder) will generate random number
        // in range from 0 to 127

        // each task has its own overwriting buffer
        thread_local std::vector<char> buffer;

        /* writeAndFlush will overwrite block of the file
         * or even entire file with buffer, and flush that data into that file
         */
        auto writeAndFlush([&fout = fout, &buffer = buffer]
            (long long startingPoint) {
            fout.seekp(startingPoint, std::ios::beg);
            fout.write((char *) &buffer[0], buffer.size());
            fout.flush();
        });

        /* lambda that runs full overwrite process,
         * first overwrites with random data,
         * then overwrites with zeros
         */
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

        // if we're gonna overwrite file via blocks of data
        if(viaBlocks) {
            /* every task has its own starting point.
             * starting point is the place in file from which
             * we actually start overwriting data
             * it's a pointer which points to a byte in the file
             */
            thread_local auto startingPoint(permittedMemory * index);
            // resize our buffer to the size of permitted memory
            buffer.resize(permittedMemory);

            // while we didn't overwrite entire file
            while(startingPoint < fileSize) {
                // Overwrite that block of data
                processOverwrite(startingPoint);
                // move to the next block of data
                startingPoint += permittedMemory * numOfBranches;
            }
        }
        // if we're gonna overwrite entire file at once
        else {
            // resize our buffer to the size of that file
            buffer.resize(fileSize);
            // overwrite entire file
            processOverwrite(0);
        }

    });
    /* if the actual size of the file is not greater than
     * permitted processing memory
     */
    if(fileSize <= permittedMemory)
        // then we can overwrite that file in existing thread
        overwrite(false, 0);

    // otherwise
    else {
        // overwrite it via blocks of data, in different tasks(threads)
        std::vector<std::future<void>> overwritingTasks;
        // index of task
        auto taskIndex(0);
        // while we don't have 4 tasks
        while(taskIndex <= 3) {
            // create task
            overwritingTasks.push_back(std::async(overwrite, true, taskIndex));
            // increment task index for indexing next task
            ++taskIndex;
        }

        // wait for each task to finish
        for(auto& eachTask: overwritingTasks)
            eachTask.wait();
    }
    // close file
    fout.close();
    // open it again to free garbage data which it contains
    fout.open(m_fileToShred.generic_string(), std::ios::binary);
    // close it again
    fout.close();
    // at this point we finished file overwriting
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
