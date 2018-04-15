#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <string>
#include <map>

#include "Logger/Logger.hpp"

namespace Core
{
	extern const int VERSION_PRIMARY;
	extern const int VERSION_SECONDARY;
	extern const int VERSION_MINOR;

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

	// This works and LOG_FORMAT doesn't because all the variables below are used after calling main.
	// The logger is constructed in Core.cpp and hence variables in Logger/Core.cpp might not have been
	// constructed by then.
	// Once main file is compiled ( linked ), all variables are constructed, hence it works correctly.
	// Variables are constructed at linkage time, NOT compile time.
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

	int AutoClean( const std::vector< std::string > & args );
}

#endif // CORE_HPP