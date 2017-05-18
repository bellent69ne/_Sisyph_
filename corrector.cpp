#include "corrector.hpp"

void Corrector::usage() {
	if(m_fullParams.size() == 0)
		std::cout << "Usage: ./Sisyph whatToDo(-e, -d) encryption_key(-16, -32, or the actual_key) fullPath";
}