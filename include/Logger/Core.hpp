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

static const char FMT_SPECIFIER_BEGIN = '%';
static const char FMT_SPECIFIER_END = '%';

static const std::string DEFAULT_TIMEDATE_FORMAT = "%ds% %MS% %D% %YS% - %HH%:%MM%:%SS%";
static const std::string DEFAULT_LOG_FORMAT = "%t% -> %s% : %l%";

static const int DEFAULT_MAX_LOGS_PER_ITERATION = 400;

static const std::string ERRORSTR[ ( int )Errors::LAST ] = {
	"Success",
	"Unknown Failure",
	"Undefined Format Specifier",
	"Variable Not Set",
	"File cannot be openend",
};

static Errors last_error;
static char last_error_str[ 1024 ];

Errors SetLastError( const Errors & err, const std::string & err_str );
Errors GetLastError();
std::string GetLastErrorStr();

#endif // ERRORS_HPP