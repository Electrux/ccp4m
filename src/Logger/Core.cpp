#include <iostream>
#include <string>

#include "../../include/Logger/Core.hpp"

Errors SetLastError( const Errors & err, const std::string & err_str )
{
	last_error = err;
	last_error_str = err_str + "";

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