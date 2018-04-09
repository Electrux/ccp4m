#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <thread>

#include "../../include/Logger/Core.hpp"
#include "../../include/Logger/LogLevels.hpp"
#include "../../include/Logger/TimeManager.hpp"

#include "../../include/Logger/Logger.hpp"

std::string Logger::ReplaceSpecifierByInformation( const std::string & specifier, const TimedString & logmsg )
{
	if( specifier == "t" )
		return this->timemgr.GetFormattedDateTime( & logmsg.time );

	if( specifier == "s" )
		return logmsg.sections.empty() ? "[ Default ]" : logmsg.sections;

	if( specifier == "l" )
		return logmsg.data;

	SetLastError( Errors::UNDEFINED_FORMAT_SPECIFIER, "[ Logger ][ ReplaceSpecifierByInformation ]: Used unknown format specifier: "
			+ specifier + "." );

	return "";
}

std::string Logger::GetFormattedLogString( const TimedString & logmsg )
{
	// No lock guard here because this function is called by BeginLogging which locks the mutex itself.
	std::string fmtdt = this->logformat;
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

			std::string replstr = ReplaceSpecifierByInformation( tmpstr, logmsg );

			if( replstr.empty() ) {
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

	return fmtdt;
}

void Logger::InternalBeginLogging()
{
	std::fstream file;
	if( this->logfile ) {
		file.open( this->filename, std::ios::out | std::ios::app );
		if( file )
			file << "---------BEGIN: " << this->timemgr.GetFormattedDateTime() << "-----------\n" << std::endl;
	}

	using namespace std::chrono_literals;

	while( true ) {
		std::this_thread::sleep_for( 250ms );
		std::lock_guard< std::mutex > mtx_guard( mtx );
		if( !this->continue_logging )
			break;

		if( this->logfile && !file ) {
			file.open( this->filename, std::ios::out | std::ios::app );
			if( file )
				file << "---------BEGIN: " << this->timemgr.GetFormattedDateTime() << "-----------\n" << std::endl;
		}
		
		if( !this->logfile && file ) {
			file << "\n-----------END: " << this->timemgr.GetFormattedDateTime() << "-----------\n" << std::endl;
			file.close();
		}

		int ctr = 0;
		while( !this->logstrings.empty() && ctr < this->max_logs_per_iter ) {
			std::string fmtstr = this->GetFormattedLogString( * logstrings.begin() );

			if( this->logfile ) {
				file << fmtstr << std::endl;
			}

			if( this->logconsole ) {
				if( this->logconsolelocation == LogConsoleLocation::COUT )
					std::cout << fmtstr << std::endl;
				else
					std::cerr << fmtstr << std::endl;
			}

			logstrings.erase( logstrings.begin() );
			++ctr;
		}
	}

	std::cout << "Exited log loop" << std::endl;

	std::lock_guard< std::mutex > mtx_guard( mtx );
	while( !this->logstrings.empty() ) {
		std::string fmtstr = this->GetFormattedLogString( * logstrings.begin() );
		if( fmtstr.empty() ) {
			fmtstr = "Log string error!";
		}

		if( this->logfile ) {
			file << fmtstr << std::endl;
		}

		if( this->logconsole ) {
			if( this->logconsolelocation == LogConsoleLocation::COUT )
				std::cout << fmtstr << std::endl;
			else
				std::cerr << fmtstr << std::endl;
		}

		logstrings.erase( logstrings.begin() );
	}

	if( file ) {
		file << "\n-----------END: " << this->timemgr.GetFormattedDateTime() << "-----------\n" << std::endl;
		file.close();
	}

	// This should already be false since only when it is false will the loop exit.
	// But still, for the sake of safety.
	this->continue_logging = false;

	SetLastError( Errors::SUCCESS, "[ Logger ][ InternalBeginLogging ]: Logging finished. Exiting async execution." );
}

Logger::Logger() : logformat( DEFAULT_LOG_FORMAT ), sections( "" )
{
	loglevel = LogLevels::ALL;

	max_logs_per_iter = DEFAULT_MAX_LOGS_PER_ITERATION;

	logfile = false;
	logconsole = true;
	logconsolelocation = LogConsoleLocation::CERR;

	continue_logging = false;
}

Logger::~Logger()
{
	{
		std::lock_guard< std::mutex > mtx_guard( mtx );
		this->continue_logging = false;
	}
	for( auto & t : threadpool )
		t.join();
}

void Logger::AddLogStrings( const LogLevels & loglevel, const std::vector< std::string > & logstrs )
{
	if( ( int )this->loglevel < ( int )loglevel )
		return;
	// Time before lock guard since we can't be sure how long lock guard will take to aquire mutex.
	auto currtime = std::time( NULL );
	std::lock_guard< std::mutex > mtx_guard( mtx );
	for( auto logstr : logstrs )
		logstrings.push_back( { logstr, currtime, this->sections } );
}

void Logger::AddLogString( const LogLevels & loglevel, const std::string & logstr )
{
	if( ( int )this->loglevel < ( int )loglevel )
		return;
	// Time before lock guard since we can't be sure how long lock guard will take to aquire mutex.
	auto currtime = std::time( NULL );
	std::lock_guard< std::mutex > mtx_guard( mtx );
	logstrings.push_back( { logstr, currtime, this->sections } );
}

bool Logger::BeginLogging()
{
	if( this->continue_logging ) {
		SetLastError( Errors::VAR_NOT_SET, "[ Logger ][ BeginLogging ]: Logger is already running." );
		return false;
	}

	if( this->filename.empty() && this->logfile ) {
		SetLastError( Errors::VAR_NOT_SET,
			"[ Logger ][ BeginLogging ]: Log file is not set. Do so using Logger::SetLogFile function." );
		return false;
	}

	if( this->logformat.empty() ) {
		SetLastError( Errors::VAR_NOT_SET,
			"[ Logger ][ BeginLogging ]: No log format specified. Do so using Logger::SetLogFormat function." );
		return false;
	}

	if( !this->logfile && !this->logconsole ) {
		SetLastError( Errors::VAR_NOT_SET,
			"[ Logger ][ BeginLogging ]: Nowhere to log data. Both logfile and logconsole are disabled."
			" Enable at least one using Logger::SetLogInFile / Logger::SetLogOnConsole functions." );
		return false;
	}

	if( this->logfile ) {
		std::fstream testfile;
		testfile.open( this->filename, std::ios::out | std::ios::app );
		if( !testfile ) {
			SetLastError( Errors::FILE_NOT_OPEN, "[ Logger ][ BeginLogging ]: Unable to open file for logging: " + this->filename +
				". Cannot begin logging." );
			return false;
		}
	}

	this->continue_logging = true;

	threadpool.emplace_back( & Logger::InternalBeginLogging, this );

	SetLastError( Errors::VAR_NOT_SET,
			"[ Logger ][ BeginLogging ]: Started Logger thread." );

	return true;
}

bool Logger::EndLogging()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );

	if( this->continue_logging ) {
		this->continue_logging = false;
		return true;
	}

	return false;
}

bool Logger::IsLogging()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->continue_logging;
}

void Logger::SetMaxLogsPerIteration( const int & max_logs )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->max_logs_per_iter = max_logs;
}

int Logger::GetMaxLogsPerIteration()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->max_logs_per_iter;
}

void Logger::SetLogFile( const std::string & filename )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->filename = filename;
	this->logfile = true;
}

std::string Logger::GetLogFile()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->filename;
}

void Logger::SetLogFormat( const std::string & format )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->logformat = format;
}

std::string Logger::GetLogFormat()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->logformat;
}

void Logger::AddLogSection( const std::string & section )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );

	this->nestedsections.push_back( section );

	this->sections = "[ ";
	for( auto it = this->nestedsections.begin(); it != this->nestedsections.end(); ++it ) {
		if( it == this->nestedsections.end() - 1 ) {
			this->sections += * it + " ]";
			continue;
		}

		this->sections += * it + " ][ ";
	}
}

std::string Logger::GetLastLogSection()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );

	if( this->nestedsections.empty() )
		return "";

	return * ( this->nestedsections.end() - 1 );
}

bool Logger::RemoveLastLogSection()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );

	if( this->nestedsections.empty() )
		return false;

	this->nestedsections.pop_back();

	this->sections = "[ ";
	for( auto it = this->nestedsections.begin(); it != this->nestedsections.end(); ++it ) {
		if( it == this->nestedsections.end() - 1 ) {
			this->sections += * it + " ]";
			continue;
		}

		this->sections += * it + " ][ ";
	}

	return true;
}

void Logger::SetLogOnConsole( bool val )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->logconsole = val;
}

bool Logger::GetLogOnConsole()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->logconsole;
}

void Logger::SetLogInFile( bool val )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->logfile = val;
}

bool Logger::GetLogInFile()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->logfile;
}

void Logger::SetLogLevel( const LogLevels & loglevel )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->loglevel = loglevel;
}

LogLevels Logger::GetLogLevel()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->loglevel;
}

void Logger::SetLogConsoleLocation( const LogConsoleLocation & location )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->logconsolelocation = location;
}

LogConsoleLocation Logger::GetLogConsoleLocation()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->logconsolelocation;
}

void Logger::SetTimeFormat( const std::string & format )
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	this->timemgr.SetFormat( format );
}

std::string Logger::GetTimeFormat()
{
	std::lock_guard< std::mutex > mtx_guard( mtx );
	return this->timemgr.GetFormat();
}
