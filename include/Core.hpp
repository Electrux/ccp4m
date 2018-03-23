#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <map>

#include "Logger/Logger.hpp"

namespace Core
{
	extern const std::string ERR_STR;
	extern const std::string FILE_TIME_FORMAT;

	extern std::map< std::string, std::string > COLORS;

	extern Logger logger;

	bool ReturnBool( bool val );
	int ReturnInt( int val );

	bool InitCore();

	bool InitLogger( const std::string & file );

	bool EndLogger();

	std::string GetCurrDateTime();
}

#endif // CORE_HPP