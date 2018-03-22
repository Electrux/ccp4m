#include <iostream>

#include "../include/DateTime.hpp"
#include "../include/GlobalData.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/StringFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/Helps.hpp"
#include "../include/ProjectManager.hpp"
#include "../include/Core.hpp"
#include "../include/Project/Config.hpp"

int Exit( int err_code )
{
	Global::EndLogger();
	return err_code;
}

int main( int argc, char ** argv )
{
	std::string logfile = DT::GetCurrDateTime() + ".log";
	FS::CreateFile( logfile );
	if( !Global::InitLogger( logfile ) ) {
		Display( "{br}Failed to initialize logging engine!{0}" );
		return 1;
	}

	auto args = Str::DoublePtrToVector( argc, ( const char ** & )argv );

	if( !Core::Init() )
		return Exit( 1 );

	int err_code = 0;

	ProjectConfig conf;
	conf.LoadFile( "yaml_test.yaml" );
	conf.DisplayAll();

	if( args.size() < 2 ) {
		Helps::Usage( args );
		err_code = 0;
	}

	if( args[ 1 ] == "help" ) {
		Helps::Usage( args );
		err_code = 0;
	}

	if( args[ 1 ] == "project" ) {
		err_code = Project::Handle( args );
	}

	return Exit( err_code );
}