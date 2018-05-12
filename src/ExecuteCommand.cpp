#include <cstdio>
#include <string>
#include <atomic>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/ExecuteCommand.hpp"

std::atomic< int > Exec::Internal::threadctr( 0 );

int Exec::ExecuteCommand( const std::string & cmd, std::string * err )
{
	std::string cmdfinal = cmd + " 2>" + Core::TMP_FILE;

	Core::logger.AddLogSection( "Exec" );
	Core::logger.AddLogSection( "ExecuteCommand" );

	Core::logger.AddLogString( LogLevels::ALL, "Executing: " + cmd );

	int res = std::system( cmdfinal.c_str() );

	if( FS::LocExists( Core::TMP_FILE ) ) {
		if( err != nullptr ) * err = FS::ReadFile( Core::TMP_FILE );
		FS::DeleteFile( Core::TMP_FILE );
	}

	return Core::ReturnVar( res );
}

Exec::Internal::Result Exec::MultithreadedExecute( const std::string & cmd, const std::string & src )
{
	std::string tmp_file = Core::TMP_FILE + std::to_string( Internal::threadctr );
	std::string cmdfinal = cmd + " 2>" + tmp_file;

	// No log section here because multiple threads would spawn multiple sections
	Core::logger.AddLogString( LogLevels::ALL, "Exec::MultithreadedExecute: Executing: " + cmdfinal );

	int res;
	std::string err;

	FILE * pipe = popen( cmdfinal.c_str(), "r" );

	if( !pipe ) return { 1, "Pipe open failed", src };

	res = pclose( pipe );

	if( FS::LocExists( tmp_file ) ) {
		err = FS::ReadFile( tmp_file );
		FS::DeleteFile( tmp_file );
	}

	--Internal::threadctr;
	return { res, err, src };
}
