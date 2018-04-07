#include <iostream>

#include "../include/FSFuncs.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/Environment.hpp"
#include "../include/Helps.hpp"
#include "../include/ProjectManager.hpp"
#include "../include/Core.hpp"
#include "../include/Vars.hpp"
#include "../include/Logger/Core.hpp"

int Exit( int err_code )
{
	Core::EndLogger();
	return err_code;
}

int main( int argc, char ** argv )
{
	auto args = Str::DoublePtrToVector( argc, ( const char ** & )argv );

	if( args.size() > 1 && args[ 1 ] == "clean" ) {
		Display( "\n" );
		return Core::AutoClean( args );
	}

	std::string logfile = Env::CCP4M_LOG_DIR + "/" + Core::GetCurrDateTime() + ".log";
	FS::CreateFileIfNotExists( logfile );
	if( !Core::InitLogger( logfile ) ) {
		Display( "{br}Failed to initialize logging engine! + {fc}" + GetLastErrorStr() + "{0}\n" );
		return 1;
	}

	if( !Core::InitCore() )
		return Exit( 1 );

	Vars::Initialize();

	int err_code = 0;

	if( args.size() < 2 ) {
		Helps::Usage( args );
		return Exit( 0 );
	}

	Display( "\n" );

	if( args[ 1 ] == "help" ) {
		Helps::Usage( args );
		err_code = 0;
	}

	if( args[ 1 ] == "project" ) {
		err_code = Project::Handle( args );
	}
	else {
		Display( "{fc}Unknown subcommand found{0}: {r}" + args[ 1 ] + "{0}\n" );
		err_code = 1;
	}

	return Exit( err_code );
}