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
	//std::shared_ptr<fs::path> m_fileToShred;
	fs::path *m_fileToShred;
public:
	template<typename T,
			 typename = std::enable_if<
			 				std::is_pointer<T>::value &&
			 				std::is_constructible<fs::path,
			 					std::remove_pointer_t<T>
			 				>::value
			 			>
	>
	explicit Shredder(T* pathToFile):
					m_fileToShred(pathToFile) {
	}

	auto randomRename() {
		constexpr auto maxRenameAttempts(10);
		auto attempts(0);

		while(attempts <maxRenameAttempts) {
			auto newPath(static_cast<const fs::path>(m_fileToShred->parent_path() 
									/ fs::unique_path().generic_string()));
			if(!fs::exists(newPath)) {
				boost::system::error_code ec;
				fs::rename(*m_fileToShred, newPath, ec);
				if(ec)
					std::cerr << "Failed renaming " << fs::absolute(*m_fileToShred)
							  << " to " << fs::absolute(newPath) << std::endl;

				else
					return newPath;
			}
		}
	}

	auto writeRandomData() {
		boost::system::error_code ec;
		const auto fileSize(fs::file_size(*m_fileToShred, ec));
		if(ec) {
			std::cerr << "Failed determining the size of " 
					  << fs::absolute(*m_fileToShred) << ec.message() << std::endl;
			return false;
		}

		std::ofstream fout(m_fileToShred->generic_string(), std::ios::binary);
		if(!fout) {
			std::cerr << "Failed opening " << fs::absolute(*m_fileToShred)
					  << strerror(errno) << std::endl;
			return false;
		}

		std::vector<unsigned char> buffer(fileSize);
		auto iterations(5);

		// Overwrite file with ASCII 255 and ASCII 0 multiple times
		while(iterations--) {
			const auto ch(static_cast<unsigned char>((iterations & 1) ? 255 : 0));
			for(auto& bufferElement: buffer)
				bufferElement = ch;

			fout.seekp(0, std::ios::beg);
			fout.write((char*) &buffer[0], buffer.size());
			fout.flush();
		}

		std::random_device seed;
		std::mt19937 randomSeeder(seed());
		std::uniform_int_distribution<> generate(0, 128);

		fout.seekp(0, std::ios::beg);
		fout.write((char*) &buffer[0], buffer.size());
		fout.flush();

		// Overwrite file with null chars
		for(auto& bufferElement: buffer)
			bufferElement = 0;

		fout.seekp(0, std::ios::beg);
		fout.write((char*) &buffer[0], buffer.size());
		fout.flush();

		// Change file size to 0
		fout.close();
		fout.open(m_fileToShred->generic_string(), std::ios::binary);

		fout.close();
		return true;
	}

	template<typename T,
			 typename = std::enable_if<
			 				std::is_pointer<T>::value &&
			 				std::is_assignable<fs::path,
			 					std::remove_pointer_t<T>
			 				>::value
			 			>
	>
	void shredFile(T* newFile) {
		m_fileToShred = newFile;

		shredFile();
	}

	void shredFile();

	Shredder() = default;
};

#endif