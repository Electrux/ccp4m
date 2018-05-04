#ifndef LOGGER_LOGLEVELS_HPP
#define LOGGER_LOGLEVELS_HPP

#include <string>

enum class LogLevels
{
	MSG,
	ERROR,
	WARNING,
	ALL,

	LAST
};

enum class LogConsoleLocation
{
	COUT,
	CERR,
};

const std::string LOGLEVELSTR[ ( int )LogLevels::LAST ] = {
	"Message",
	"Error",
	"Warning",
	"All",
};

#endif // LOGGER_LOGLEVELS_HPP