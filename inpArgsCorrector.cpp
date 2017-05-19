#include "inpArgsCorrector.hpp"

void InpArgsCorrector::usage() const {
		std::cerr << "Usage: ./Sisyph -r(optional) whatToDo(-e," 
				  << "-d) encryption_key fullPath\n";
		exit(1);
}