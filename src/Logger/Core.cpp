#include <iostream>
#include <string>

#include "../../include/Logger/Core.hpp"

ErrorInfo errinf;

Errors SetLastError( const Errors & err, const std::string & err_str )
{
	errinf.last_error = err;
	errinf.last_error_str = err_str;
	std::cout << "\033[0;32mpretty much ending of SetLastError()\033[0m" << std::endl;

	return errinf.last_error;
}

Errors GetLastError()
{
	return errinf.last_error;
}

std::string GetLastErrorStr()
{
	return errinf.last_error_str;
}