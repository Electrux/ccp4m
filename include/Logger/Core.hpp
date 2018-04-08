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

const static char FMT_SPECIFIER_BEGIN = '%';
const static char FMT_SPECIFIER_END = '%';

const static std::string DEFAULT_TIMEDATE_FORMAT = "%ds% %MS% %D% %YS% - %HH%:%MM%:%SS%";
const static std::string DEFAULT_LOG_FORMAT = "%t% -> %s% : %l%";

const static int DEFAULT_MAX_LOGS_PER_ITERATION = 400;

const static std::string ERRORSTR[ ( int )Errors::LAST ] = {
	"Success",
	"Unknown Failure",
	"Undefined Format Specifier",
	"Variable Not Set",
	"File cannot be openend",
};

static Errors last_error;
static std::string last_error_str;

void InitVars();

Errors SetLastError( const Errors & err, const std::string & err_str );
Errors GetLastError();
std::string GetLastErrorStr();

#endif // ERRORS_HPP