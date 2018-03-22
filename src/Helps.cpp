#include <vector>
#include <string>

#include "../include/DisplayFuncs.hpp"

#include "../include/Helps.hpp"

void Helps::Usage( const std::vector< std::string > & args )
{
	Display( "{fc}Usage:\n" );

	Display( "\t{sc}" + args[ 0 ] + "{tc} {{ project / package / help }\n\n" );

	Display( "{fc}Use {sc}" + args[ 0 ] + "{tc} < arg > help {fc}for more information{0}\n" );
}

void Helps::Project( const std::vector< std::string > & args )
{
	Display( "{fc}Usage:\n" );

	Display( "\t{sc}" + args[ 0 ] + " project {tc}{{ new / build / run / FLAG_OPTS }\n\n" );

	Display( "\t\t{fc}Arguments are:\n" );

	Display( "\t\t\t{sc}new {0}-{tc} Create a new project\n" );
	Display( "\t\t\t{sc}build {0}-{tc} Build a project {lc}You must be in it\'s directory\n" );
	Display( "\t\t\t{sc}run {0}-{tc} Build a project and run it {lc}You must be in it\'s directory\n\n" );

	Display( "{fc}Use {sc}" + args[ 0 ] + "project {tc}< arg > help {fc}for more information{0}\n" );
}