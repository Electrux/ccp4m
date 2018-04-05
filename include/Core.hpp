#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <string>
#include <map>

#include "Logger/Logger.hpp"

namespace Core
{
	enum Arch
	{
		LINUX,
		MAC,
		BSD,
		OTHER
	};

	extern Arch arch;

	void SetVarArch( std::string & var, const std::vector< std::string > & opts );
	void SetVarArch( int & var, const std::vector< int > & opts );

	extern const std::string ERR_STR;
	extern const std::string FILE_TIME_FORMAT;

	extern const std::string TMP_FILE;

	extern std::map< std::string, std::string > COLORS;

	extern Logger logger;

	void ReturnVoid();

	// Template class/function definitions cant be in separate source files
	template< class T > T ReturnVar( T val )
	{
		// Remove function and class / namespace
		logger.RemoveLastLogSection();
		logger.RemoveLastLogSection();
		return val;
	}

	bool InitCore();

	bool InitLogger( const std::string & file );

	bool EndLogger();

	std::string GetCurrDateTime();
}

#endif // CORE_HPP