#ifndef GLOBAL_DATA_HPP
#define GLOBAL_DATA_HPP

#include <string>

#include "../include/Logger/Logger.hpp"

namespace Global
{
	extern const std::string ERR_STR;
	extern const std::string FILE_TIME_FORMAT;

	extern Logger logger;

	bool InitLogger( const std::string & file );

	bool EndLogger();
}

#endif // GLOBAL_DATA_HPP