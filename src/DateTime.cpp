#include <string>

#include "../include/Logger/TimeManager.hpp"

#include "../include/GlobalData.hpp"

#include "../include/DateTime.hpp"

std::string DT::GetCurrDateTime()
{
	TimeManager tm;

	tm.SetFormat( Global::FILE_TIME_FORMAT );

	return tm.GetFormattedDateTime();
}