#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <vector>
#include <mutex>
#include <thread>

#include "Core.hpp"
#include "LogLevels.hpp"
#include "TimeManager.hpp"

class Logger
{
	struct TimedString
	{
		std::string data;
		std::time_t time;
		std::string sections;
	};

	std::mutex mtx;

	LogLevels loglevel;

	int max_logs_per_iter;

	bool logfile;
	bool logconsole;
	LogConsoleLocation logconsolelocation;

	bool continue_logging;

	std::string logformat;

	TimeManager timemgr;

	std::string filename;

	std::vector< std::string > nestedsections;
	std::string sections;

	std::vector< TimedString > logstrings;

	std::vector< std::thread > threadpool;

	std::string ReplaceSpecifierByInformation( const std::string & specifier, const TimedString & logmsg );
	std::string GetFormattedLogString( const TimedString & logmsg );

	void InternalBeginLogging();

public:

	Logger();
	~Logger();

	void AddLogStrings( const LogLevels & loglevel, const std::vector< std::string > & logstrs );
	void AddLogString( const LogLevels & loglevel, const std::string & logstr );

	bool BeginLogging();
	bool EndLogging();
	bool IsLogging();

	void SetMaxLogsPerIteration( const int & max_logs );
	int GetMaxLogsPerIteration();

	void SetLogFile( const std::string & file );
	std::string GetLogFile();

	void SetLogFormat( const std::string & format );
	std::string GetLogFormat();

	void AddLogSection( const std::string & section );
	std::string GetLastLogSection();

	bool RemoveLastLogSection();

	void SetLogOnConsole( bool val );
	bool GetLogOnConsole();

	void SetLogInFile( bool val );
	bool GetLogInFile();

	void SetLogLevel( const LogLevels & loglevel );
	LogLevels GetLogLevel();

	void SetLogConsoleLocation( const LogConsoleLocation & location );
	LogConsoleLocation GetLogConsoleLocation();

	void SetTimeFormat( const std::string & format );
	std::string GetTimeFormat();
};

#endif // LOGGER_HPP