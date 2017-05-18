#include <string>
#include <iostream>
#include <vector>

class Corrector {
private:
	std::vector<std::string> m_fullParams;

public:
	template<typename T, typename paramT,
			 typename = std::enable_if<
			 	std::is_integral<T>::value &&
			 	std::is_constructible<std::string, 
			 		paramT
			 	>::value
			>
	>
	Corrector(T&& paramLength, paramT** params):
							m_fullParams(params, params + paramLength ) {
		//for(auto iii(0); iii < m_fullParams.size(); ++iii) {
		//	m_fullParams[iii] = params[iii];
			//std::cout << m_fullParams[iii] << std::endl;
//		}
	}

	inline auto wannaEncrypt() {
		if(m_fullParams.at(1) == "-e")
			return true;
		else if(m_fullParams.at(1) == "-d")
			return false;

		usage();
		exit(1);
	}

	inline auto actualKey() {
		return m_fullParams.at(2);
	}

//	inline

	inline auto path() {
		return m_fullParams.at(3);
	}

	void usage();
};