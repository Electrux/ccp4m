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

extern const char FMT_SPECIFIER_BEGIN;
extern const char FMT_SPECIFIER_END;

extern const std::string DEFAULT_TIMEDATE_FORMAT;
extern const std::string DEFAULT_LOG_FORMAT;

extern const int DEFAULT_MAX_LOGS_PER_ITERATION ;

extern const std::string ERRORSTR[ ( int )Errors::LAST ];

static Errors last_error;
static std::string last_error_str;

Errors SetLastError( const Errors & err, const std::string & err_str );
Errors GetLastError();
std::string GetLastErrorStr();

#endif // ERRORS_HPP