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
	Vars::DeleteSingleton();
	Core::EndLogger();
	Display( "\n" );
	return err_code;
}

int main( int argc, char ** argv )
{
	auto && args = Str::DoublePtrToVector( argc, ( const char ** & )argv );

	if( args.size() > 1 && args[ 1 ] == "clean" ) {
		Display( "\n" );
		int res = Core::AutoClean( args );
		Display( "\n" );
		return res;
	}

	std::string logfile = Env::CCP4M_LOG_DIR + "/" + Core::GetCurrDateTime() + ".log";
	FS::CreateFileIfNotExists( logfile );
	if( !Core::InitLogger( logfile ) ) {
		Display( "\n{br}Failed to initialize logging engine{0}! {fc}" + GetLastErrorStr() + "{0}\n\n" );
		return 1;
	}

	Display( "\n" );

	if( !Core::InitCore() ) return Exit( 1 );

	Vars::Initialize();

	int err_code = 0;

	if( args.size() < 2 ) {
		Helps::Usage( args );
		return Exit( 0 );
	}
	else if( args[ 1 ] == "help" ) {
		Helps::Usage( args );
		err_code = 0;
	}
	else if( args[ 1 ] == "--version" || args[ 1 ] == "version" ) {
		Display( "{fc}Running Version{0}: {sc}" + std::to_string( Core::VERSION_PRIMARY ) + "{0}.{sc}" +
			 std::to_string( Core::VERSION_SECONDARY ) + "{0}.{sc}" + std::to_string( Core::VERSION_MINOR ) + "{0}\n" );
		err_code = 0;
	}
	else if( args[ 1 ] == "project" ) {
		err_code = Project::Handle( args );
	}
	else {
		Display( "{fc}Unknown subcommand found{0}: {r}" + args[ 1 ] + "{0}\n" );
		err_code = 1;
	}

	return Exit( err_code );
}
