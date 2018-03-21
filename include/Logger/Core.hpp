#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>

enum class Errors
{
	SUCCESS,
	UNKNOWN_FAILURE,
	UNDEFINED_FORMAT_SPECIFIER,
	VAR_NOT_SET,
	FILE_NOT_OPEN,

	LAST
};

const char FMT_SPECIFIER_BEGIN = '%';
const char FMT_SPECIFIER_END = '%';

const std::string DEFAULT_TIMEDATE_FORMAT = "%ds% %MS% %D% %YS% - %HH%:%MM%:%SS%";
const std::string DEFAULT_LOG_FORMAT = "%t% -> %s% : %l";

const int DEFAULT_MAX_LOGS_PER_ITERATION = 400;

const std::string ERRORSTR[ ( int )Errors::LAST ] = {
	"Success",
	"Unknown Failure",
	"Undefined Format Specifier",
	"Variable Not Set",
	"File cannot be openend",
};

static Errors last_error;
static std::string last_error_str;

Errors SetLastError( const Errors & err, const std::string & err_str );
Errors GetLastError();
std::string GetLastErrorStr();

#endif // ERRORS_HPP