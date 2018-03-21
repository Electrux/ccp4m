#ifndef LOGLEVELS_HPP
#define LOGLEVELS_HPP

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

#endif // LOGLEVELS_HPP