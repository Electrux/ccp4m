#include <cstdio>
#include <string>
#include <array>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/ExecuteCommand.hpp"

int Exec::ExecuteCommand( const std::string & cmd, std::string * err, std::string * out, bool disp_out, int thread )
{
	std::string tmp_file = Core::TMP_FILE + std::to_string( thread );

	std::string cmdfinal = cmd + " 2>" + tmp_file;

	std::array< char, 256 > buffer;

	Core::logger.AddLogSection( "Exec" );
	Core::logger.AddLogSection( "ExecuteCommand" );

	Core::logger.AddLogString( LogLevels::ALL, "Executing: " + cmd );

	FILE * pipe = popen( cmd.c_str(), "r" );
	if( !pipe ) {
		Core::logger.AddLogString( LogLevels::ALL, "Failed to open external command pipe" );
		return Core::ReturnVar( -1 );
	}

	while( !feof( pipe ) ) {
		if( fgets( buffer.data(), 256, pipe ) != nullptr && out != nullptr ) {
			* out += buffer.data();
		}
	}

	if( err != nullptr && FS::LocExists( tmp_file ) )
		* err = FS::ReadFile( tmp_file );

	return Core::ReturnVar( pclose( pipe ) );
}