#include <vector>
#include <string>

#include "../include/Helps.hpp"

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
		//err_code = Project::Build( args );
	}
	else if( args[ 2 ] == "run" ) {
		//err_code = Project::Run( args );
	}
	else {
		//err_code = Project::HandleFlags( args );
	}

	return 0;
}