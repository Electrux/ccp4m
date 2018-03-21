#include <iostream>
#include <string>
#include <ctime>

#include "../../include/Logger/Core.hpp"

#include "../../include/Logger/TimeManager.hpp"

std::string TimeManager::ReplaceSpecifierByTime( const std::time_t * time, const std::string & fmtspecifier )
{
	std::tm * t = std::localtime( time );
	if( fmtspecifier == "HH" )
		return std::to_string( t->tm_hour );

	if( fmtspecifier == "MM" )
		return std::to_string( t->tm_min );

	if( fmtspecifier == "SS" )
		return std::to_string( t->tm_sec );

	if( fmtspecifier == "D" )
		return std::to_string( t->tm_mday );

	if( fmtspecifier == "d" )
		return std::to_string( t->tm_wday );

	if( fmtspecifier == "ds" )
		return DAYS[ t->tm_wday ];

	if( fmtspecifier == "dss" )
		return DAYS_SHORT[ t->tm_wday ];

	if( fmtspecifier == "M" )
		return std::to_string( t->tm_mon );

	if( fmtspecifier == "MS" )
		return MONTHS[ t->tm_mon ];

	if( fmtspecifier == "MSS" )
		return MONTHS_SHORT[ t->tm_mon ];

	if( fmtspecifier == "Y" )
		return std::to_string( t->tm_year + 1900 );

	if( fmtspecifier == "YS" )
		return std::to_string( t->tm_year + 1900 ).substr( 2 );

	SetLastError( Errors::UNDEFINED_FORMAT_SPECIFIER,
		"[ TimeManager ][ ReplaceSpecifierByTime ]: Used unknown format specifier: " + fmtspecifier + "." );

	return "";
}

TimeManager::TimeManager()
{
	this->format = DEFAULT_TIMEDATE_FORMAT;
}

void TimeManager::SetFormat( const std::string & formatstr )
{
	this->format = formatstr;
}

std::string TimeManager::GetFormat()
{
	return this->format;
}

std::string TimeManager::GetFormattedDateTime( const std::time_t * const_time )
{
	bool new_time_alloc = false;

	std::time_t * time = ( std::time_t * )const_time;
	if( time == nullptr ) {
		time = new std::time_t;
		* time = std::time( nullptr );
		new_time_alloc = true;
	}

	std::string fmtdt = this->format;

	std::string tmpstr;
	for( auto it = fmtdt.begin(); it != fmtdt.end(); ) {
		if( * it == FMT_SPECIFIER_BEGIN ) {
			it = fmtdt.erase( it );
			while( it != fmtdt.end() && * it != FMT_SPECIFIER_END ) {
				tmpstr += * it;
				it = fmtdt.erase( it );
			}
			if( !tmpstr.empty() )
				it = fmtdt.erase( it );

			std::string replstr = ReplaceSpecifierByTime( time, tmpstr );
			if( replstr.empty() ) {
				if( new_time_alloc )
					delete time;
				return "";
			}

			it = fmtdt.insert( it, replstr.begin(), replstr.end() );

			it += replstr.size();
			tmpstr.clear();
		}
		else {
			++it;
		}
	}

	if( new_time_alloc )
		delete time;

	return fmtdt;
}