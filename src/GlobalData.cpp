#include <string>

#include "../include/FSFuncs.hpp"

#include "../include/Logger/LogLevels.hpp"
#include "../include/Logger/Logger.hpp"

#include "../include/GlobalData.hpp"

const std::string Global::ERR_STR = "__ERR__";
const std::string Global::FILE_TIME_FORMAT = "%ds%.%MS%.%D%.%YS%-%HH%:%MM%:%SS%";

Logger Global::logger;

bool Global::InitLogger( const std::string & file )
{
	logger.SetMaxLogsPerIteration( 25 );

	logger.SetLogLevel( LogLevels::ALL );

	logger.SetLogOnConsole( false );
	logger.SetLogInFile( true );

	logger.SetLogFile( file );

	return logger.BeginLogging();
}

bool Global::EndLogger()
{
	return logger.IsLogging() ? logger.EndLogging() : false;
}