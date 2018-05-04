#ifndef LOGGER_TIMEMANAGER_HPP
#define LOGGER_TIMEMANAGER_HPP

#include <string>
#include <ctime>

#include "Core.hpp"

const std::string DAYS[ 7 ] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

const std::string MONTHS[ 12 ] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

const std::string DAYS_SHORT[ 7 ] = {
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

const std::string MONTHS_SHORT[ 12 ] = {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

class TimeManager
{
	std::string format;
	std::string ReplaceSpecifierByTime( const std::time_t * time, const std::string & fmtspecifier );
public:
	TimeManager();
	void SetFormat( const std::string & formatstr );
	std::string GetFormat();

	std::string GetFormattedDateTime( const std::time_t * const_time = nullptr );
};

#endif // LOGGER_TIMEMANAGER_HPP