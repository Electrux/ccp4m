#include <cstdio>
#include <vector>
#include <string>
#include <array>
#include <thread>
#include <future>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/ExecuteCommand.hpp"

std::vector< Exec::Internal::Result > Exec::Internal::threadinfo;

int Exec::ExecuteCommand( const std::string & cmd, std::string * err )
{
	std::string cmdfinal = cmd + " 2>" + Core::TMP_FILE;

	Core::logger.AddLogSection( "Exec" );
	Core::logger.AddLogSection( "ExecuteCommand" );

	Core::logger.AddLogString( LogLevels::ALL, "Executing: " + cmd );

	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) {
		Core::logger.AddLogString( LogLevels::ALL, "Failed to open external command pipe" );
		return Core::ReturnVar( -1 );
	}

	if( err != nullptr && FS::LocExists( Core::TMP_FILE ) )
		* err = FS::ReadFile( Core::TMP_FILE );

	return Core::ReturnVar( pclose( pipe ) );
}

void Exec::MultithreadedExecute( const std::string & cmd, int thread, const std::string & src )
{
	std::string tmp_file = Core::TMP_FILE + std::to_string( thread );
	std::string cmdfinal = cmd + " 2>" + tmp_file;

	// No log section here because multiple threads would spawn multiple sections

	Core::logger.AddLogString( LogLevels::ALL, "Exec::MultithreadedExecute: Executing: " + cmd );

	int res = std::system( cmdfinal.c_str() );

	std::string err;

	if( FS::LocExists( tmp_file ) ) {
		err = FS::ReadFile( tmp_file );
		FS::DeleteFile( tmp_file );
	}

	Internal::threadinfo[ thread ].res = res;
	Internal::threadinfo[ thread ].err = err;
	Internal::threadinfo[ thread ].src = src;
}