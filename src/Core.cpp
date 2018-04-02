#include <iostream>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>
#include <curl/curl.h>

#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/Environment.hpp"
#include "../include/UTFChars.hpp"
#include "../include/Logger/Logger.hpp"
#include "../include/Logger/TimeManager.hpp"
#include "../include/Vars.hpp"

#include "../include/Core.hpp"

Core::Arch Core::arch =
#ifdef __linux__
	Core::LINUX
#elif __APPLE__
	Core::MAC
#elif __FreeBSD__
	Core::BSD
#else
	Core::OTHER
#endif
;

void Core::SetVarArch( std::string & var, const std::vector< std::string > & opts )
{
	var = opts.size() <= arch ? "" : opts[ arch ];
}

void Core::SetVarArch( int & var, const std::vector< int > & opts )
{
	var = opts.size() <= arch ? std::numeric_limits< int >().min() : opts[ arch ];
}

const std::string Core::ERR_STR = "__ERR__";
const std::string Core::FILE_TIME_FORMAT = "%ds%.%MS%.%D%.%YS%";
const std::string Core::TMP_FILE = ".ccp4mtmp";

std::map< std::string, std::string > Core::COLORS = {
	{ "0", "\033[0m" },

	{ "r", "\033[0;31m" },
	{ "g", "\033[0;32m" },
	{ "y", "\033[0;33m" },
	{ "b", "\033[0;34m" },
	{ "m", "\033[0;35m" },
	{ "c", "\033[0;36m" },
	{ "w", "\033[0;37m" },

	{ "br", "\033[1;31m" },
	{ "bg", "\033[1;32m" },
	{ "by", "\033[1;33m" },
	{ "bb", "\033[1;34m" },
	{ "bm", "\033[1;35m" },
	{ "bc", "\033[1;36m" },
	{ "bw", "\033[1;37m" },

	{ "fc", "\033[0;35m" },
	{ "sc", "\033[0;33m" },
	{ "tc", "\033[0;37m" },
	{ "lc", "\033[0;36m" },
};

Logger Core::logger;

bool Core::InitCore()
{
	bool new_config = false;

	Core::logger.AddLogSection( "Core" );
	Core::logger.AddLogSection( "Init" );

	// Also checks if location already exists
	if( !FS::CreateDir( Env::CCP4M_DIR ) ) {
		Display( "{r}Unable to create configuration directory! Please check permissions for your {fc}HOME{r} directory{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to create configuration directory: " + Env::CCP4M_DIR );
		return ReturnVar( false );
	}

	if( !FS::CreateDir( Env::CCP4M_LICENSE_DIR ) ) {
		Display( "{r}Unable to create license directory! Please check permissions for your {fc}HOME{r} directory{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to create license directory: " + Env::CCP4M_DIR );
		return ReturnVar( false );
	}

	// No need of creating log directory here since it is done by the main() function itself at the very beginning

	if( !FS::LocExists( Env::CCP4M_CONFIG_FILE ) ) {
		if( !FS::CreateFile( Env::CCP4M_CONFIG_FILE ) ) {
			Display( "{r}Unable to create configuration file! Please check permissions for directory: {fc}" + Env::CCP4M_DIR + "{0}\n" );
			return ReturnVar( false );
		}
		new_config = true;
	}

	auto v = Vars::GetSingleton();

	TimeManager tm;
	tm.SetFormat( "%Y%" );
	v->AddVar( "year", tm.GetFormattedDateTime() );

	if( !new_config ) {
		YAML::Node conf = YAML::LoadFile( Env::CCP4M_CONFIG_FILE );
		if( conf[ "author" ] )
			v->AddVar( "author", v->Replace( conf[ "author" ].as< std::string >() ) );
		return ReturnVar( true );
	}

	Display( "{fc}Initializing first time setup...\n\n" );
	Display( "{sc}Enter your name: {tc}" );

	std::string name;
	std::getline( std::cin, name );

	Display( "{sc}Enter your email: {tc}" );

	std::string email;
	std::getline( std::cin, email );

	Core::logger.AddLogString( LogLevels::ALL, "Config username: " + name + ", email: " + email );

	v->AddVar( "author", name );

	Display( "\n{fc}Generating system config...\n" );

	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << name << YAML::Comment( "Author name" );
	out << YAML::Key << "email" << YAML::Value << email << YAML::Comment( "Author email" );
	out << YAML::EndMap;

	if( !FS::CreateFile( Env::CCP4M_CONFIG_FILE, out.c_str() ) ) {
		Display( "{fc}Failed to create system config. Please check if you have correct permissions. {br}" + UTF::CROSS + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to create system configuration file: " + Env::CCP4M_CONFIG_FILE );
		return ReturnVar( false );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Successfully created system configuration file" );

	Display( "{fc}Successfully generated system config. {bg}" + UTF::TICK + "{0}\n" );
	return ReturnVar( true );
}

bool Core::InitLogger( const std::string & file )
{
	logger.SetMaxLogsPerIteration( 25 );

	logger.SetLogLevel( LogLevels::ALL );

	logger.SetLogOnConsole( false );
	logger.SetLogInFile( true );

	logger.SetLogFile( file );

	return logger.BeginLogging();
}

bool Core::EndLogger()
{
	return logger.IsLogging() ? logger.EndLogging() : false;
}

std::string Core::GetCurrDateTime()
{
	TimeManager tm;

	tm.SetFormat( Core::FILE_TIME_FORMAT );

	return tm.GetFormattedDateTime();
}