#ifndef INPARGSCORRECTOR_HPP
#define INPARGSCORRECTOR_HPP

#include <string>
#include <iostream>
#include <vector>

class InpArgsCorrector {
private:
	const short m_maxArgsNumb = 5;
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
	InpArgsCorrector(T&& paramNumb, paramT** params):
							m_fullParams(params, params + paramNumb) {
		if(paramNumb < m_maxArgsNumb - 1 && 
			paramNumb != 2)
			usage();
	}

	inline auto wannaEncrypt() {
		auto isEncryptable([&m_fullParams = m_fullParams](auto argNum) {
			if(m_fullParams.at(argNum) == "-e" || 
				m_fullParams.at(argNum) == "--encrypt")
				return true;

			else if(m_fullParams.at(argNum) == "-d" ||
					m_fullParams.at(argNum) == "--decrypt")
				return false;

			std::cerr << "Usage: ./Sisyph -r(optional) whatToDo(-e, -d) encryption_key fullPath\n";
			exit(1);		
		});

		auto isGoingEncrypt(false);
		//if(m_fullParams.size() < m_maxArgsNumb)
		m_fullParams.size() < m_maxArgsNumb ? isGoingEncrypt = isEncryptable(1)
	    									: isGoingEncrypt = isEncryptable(2);

	    return isGoingEncrypt;
	}

	inline auto actualKey() const {
		//return m_fullParams.at(2);
		if(m_fullParams.size() < m_maxArgsNumb)
			return m_fullParams.at(2);
		
		return m_fullParams.at(3);
	}

//	inline

	inline auto path() const {
		return m_fullParams.at(3);
	}

	auto isRecursive() const {
		if(m_fullParams.size() < m_maxArgsNumb)
			return false;

		auto recursive(false);
		if(m_fullParams.at(1) == "-r")
			recursive = true;
		
		else
			usage();

		return recursive;
	}

	auto actualPath() const {
		//auto inTheEnd(m_fullParams.cend());
		//return *inTheEnd;
		if(m_fullParams.size() < m_maxArgsNumb)
			return m_fullParams[m_maxArgsNumb - 2];

		return m_fullParams[m_maxArgsNumb - 1];
	}

	void usage() const;
};

#endif