#include <string>

#include "../../include/Logger/Core.hpp"

const char FMT_SPECIFIER_BEGIN = '%';
const char FMT_SPECIFIER_END = '%';

const std::string DEFAULT_TIMEDATE_FORMAT = "%ds% %MS% %D% %YS% - %HH%:%MM%:%SS%";
const std::string DEFAULT_LOG_FORMAT = "%t% -> %s% : %l%";

const int DEFAULT_MAX_LOGS_PER_ITERATION = 400;

const std::string ERRORSTR[ ( int )Errors::LAST ] = {
	"Success",
	"Unknown Failure",
	"Undefined Format Specifier",
	"Variable Not Set",
	"File cannot be openend",
};

Errors SetLastError( const Errors & err, const std::string & err_str )
{
	last_error = err;
	last_error_str = err_str;

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