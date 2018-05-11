#include <string>
#include <cstdlib>
#include <unistd.h>

#include "../include/Core.hpp"
#include "../include/StringFuncs.hpp"

#include "../include/Environment.hpp"

const std::string Env::CCP4M_DIR = std::string( std::getenv( "HOME" ) ) + "/.ccp4m";
const std::string Env::CCP4M_CONFIG_FILE = Env::CCP4M_DIR + "/config.yaml";
const std::string Env::CCP4M_LICENSE_DIR = Env::CCP4M_DIR + "/licenses";
const std::string Env::CCP4M_LOG_DIR = Env::CCP4M_DIR + "/logs";

const std::string Env::CCP4M_PROJECT_CONFIG_FILE = "ccp4m.yaml";

const std::string Env::LICENSE_URL = "https://raw.githubusercontent.com/Electrux/ccp4m-licenses/master/";

std::string Env::GetEnvVar( const std::string & key )
{
	Core::logger.AddLogSection( "Env" );
	Core::logger.AddLogSection( "GetEnvVar" );

	Core::logger.AddLogString( LogLevels::ALL, "Retrieving environment variable: " + key );

	std::string ret;

	char * res = std::getenv( key.c_str() );

	if( res != NULL ) ret = res;

	return Core::ReturnVar( ret );
}

bool Env::SetCurrentDir( const std::string & dir )
{
	Core::logger.AddLogSection( "Env" );
	Core::logger.AddLogSection( "SetCurrentDir" );

	Core::logger.AddLogString( LogLevels::ALL, "Altering current directory to: " + dir );

	if( chdir( dir.c_str() ) != 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Altering current directory failed" );
		return Core::ReturnVar( false );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Current directory altered successfully" );
	return Core::ReturnVar( true );
}

std::string Env::GetCurrentDir()
{
	Core::logger.AddLogSection( "Env" );
	Core::logger.AddLogSection( "GetCurrentDir" );

	Core::logger.AddLogString( LogLevels::ALL, "Fetching current directory" );

	char cwd[ 1024 ];

	std::string dir;

	if( getcwd( cwd, sizeof( cwd ) ) != NULL ) {
		dir = std::string( cwd );
		Core::logger.AddLogString( LogLevels::ALL, "Fetched current directory: " + dir );
	}
	else {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to fetch current directory" );
	}

	return Core::ReturnVar( dir );
}

std::vector< std::string > Env::GetEnvPathVec()
{
	Core::logger.AddLogSection( "Env" );
	Core::logger.AddLogSection( "EnvPathVec" );

	Core::logger.AddLogString( LogLevels::ALL, "Fetching PATH variable" );

	auto && res = Str::Delimit( GetEnvVar( "PATH" ), ':' );

	Core::logger.AddLogString( LogLevels::ALL, "Fetched " + std::to_string( res.size() ) + " paths" );

	Core::logger.RemoveLastLogSection();
	Core::logger.RemoveLastLogSection();

	return res;
}

void Env::ReplaceTildeWithHome( std::string & str )
{
	for( auto it = str.begin(); it != str.end(); ++it ) {
		if( * it == '~' ) {
			it = str.erase( it );
			std::string home = Env::GetEnvVar( "HOME" );
			str.insert( it, home.begin(), home.end() );
			it += home.size() - 1;
		}
	}
}
