#include <vector>
#include <string>

#include "../include/Core.hpp"
#include "../include/Helps.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/Environment.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/Project/Config.hpp"
#include "../include/Project/BuildBinary.hpp"

#include "../include/ProjectManager.hpp"

int Project::Handle( const std::vector< std::string > & args )
{
	int err_code = 0;

	if( args.size() < 3 ) {
		Helps::Project( args );
		err_code = 0;
	}
	else if( args[ 2 ] == "new" ) {
		//err_code = Project::New( args );
	}
	else if( args[ 2 ] == "build" ) {
		err_code = Project::Build();
	}
	else if( args[ 2 ] == "run" ) {
		//err_code = Project::Run( args );
	}
	else {
		//err_code = Project::HandleFlags( args );
	}

	return 0;
}

int Project::Build()
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Build" );

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnInt( 1 );
	}

	ProjectConfig pconf;

	pconf.LoadFile( Env::CCP4M_PROJECT_CONFIG_FILE );

	if( pconf.GetData().name.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name! Unable to continue." );
		return Core::ReturnInt( 1 );
	}

	if( pconf.GetData().type == "bin" ) {
		Project::BuildBinary( pconf );
	}
	else if( pconf.GetData().type == "lib" ) {

	}

	return Core::ReturnInt( 0 );
}