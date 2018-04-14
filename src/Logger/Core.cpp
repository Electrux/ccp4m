#include <iostream>
#include <string>
#include <string.h>

#include "../../include/Logger/Core.hpp"



std::string GetDefaultTimeDateFormat()
{
	static const std::string DEFAULT_TIMEDATE_FORMAT = "%ds% %MS% %D% %YS% - %HH%:%MM%:%SS%";
	return DEFAULT_TIMEDATE_FORMAT;
}

std::string GetDefaultLogFormat()
{
	static const std::string DEFAULT_LOG_FORMAT = "%t% -> %s% : %l%";
	return DEFAULT_LOG_FORMAT;
}

std::string GetErrorStrFromCode( const int which )
{
	static const std::string ERRORSTR[ ( int )Errors::LAST ] = {
		"Success",
		"Unknown Failure",
		"Undefined Format Specifier",
		"Variable Not Set",
		"File cannot be openend",
	};
	return ERRORSTR[ which ];
}

Errors SetLastError( const Errors & err, const std::string & err_str )
{
	last_error = err;
	strcpy( last_error_str, err_str.c_str() );
	return last_error;
}

Errors GetLastError()
{
	return last_error;
}

std::string GetLastErrorStr()
{
	return last_error_str;
}