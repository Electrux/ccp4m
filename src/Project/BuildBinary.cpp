#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/Project/Config.hpp"

#include "../../include/Project/BuildBinary.hpp"

int Project::BuildBinary( ProjectConfig & conf )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildExecutable" );

	Display( "{fc}Creating build directories..." );

	if( !FS::CreateDir( "buildfiles" ) || !FS::CreateDir( "lib" ) || !FS::CreateDir( "bin" ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create necessary project build directories" );
		return Core::ReturnInt( 1 );
	}

	for( auto build : conf.GetData().builds ) {
		
	}

	return Core::ReturnInt( 0 );
}