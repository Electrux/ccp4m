#include <vector>
#include <string>

#include "../include/Core.hpp"
#include "../include/Helps.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/Environment.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/UTFChars.hpp"

#include "../include/Project/Config.hpp"
#include "../include/Project/Licenses.hpp"
#include "../include/Project/BuildBinary.hpp"
#include "../include/Project/BuildLibrary.hpp"
#include "../include/Project/BuildTest.hpp"
#include "../include/Project/CodeFileGenerator.hpp"
#include "../include/Project/CreateNew.hpp"
#include "../include/Project/Add.hpp"

#include "../include/ProjectManager.hpp"

int Project::Handle( const std::vector< std::string > & args )
{
	int err_code = 0;

	if( args.size() < 3 ) {
		Helps::Project( args );
		err_code = 0;
	}
	else if( args[ 2 ] == "new" ) {
		err_code = Project::New( args );
	}
	else if( args[ 2 ] == "build" ) {
		err_code = Build( args );
	}
	else if( args[ 2 ] == "add" ) {
		err_code = Add( args );
	}
	else if( args[ 2 ] == "set" ) {
		err_code = Set( args );
	}
	else if( args[ 2 ] == "run" ) {
		err_code = Project::Run( args );
	}
	else if( args[ 2 ] == "test" ) {
		err_code = Project::Test( args );
	}
	else if( args[ 2 ] == "clean" ) {
		err_code = Project::Clean();
	}
	else if( * args[ 2 ].begin() == '-' ) {
		//err_code = Project::HandleFlags( args );
	}
	else {
		err_code = 0;
		Helps::Project( args );
	}

	return err_code;
}

int Project::Build( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Build" );

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnVar( 1 );
	}

	Display( "{fc}Creating build directories {0}...\n\n" );

	if( !FS::CreateDir( "buildfiles" ) || !FS::CreateDir( "lib" ) || !FS::CreateDir( "bin" ) || !FS::CreateDir( "testbin" ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create necessary project build directories" );
		Display( "{fc}Unable to create necessary build directories. Exiting. {br}" + UTF::CROSS + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;

	pconf.LoadFile( Env::CCP4M_PROJECT_CONFIG_FILE, false, true );

	if( pconf.GetData().name.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name! Unable to continue." );
		Display( "{fc}No project name exists in the configuration file. Unable to continue{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "\n{fc}Project version{0}: {sc}" + pconf.GetData().version + "{0}\n" );

	// User specified which build to use
	if( args.size() > 3 && args[ 3 ].substr( 0, 2 ) != "--" ) {
		std::string which_build = args[ 3 ];

		bool disp_cmds_only = args.size() > 4 && args[ 4 ] == "--cmds_only";

		int j = -1;
		for( size_t i = 0; i < pconf.GetData().builds.size(); ++i ) {
			if( pconf.GetData().builds[ i ].name == which_build ) j = i;
		}

		if( j == -1 ) {
			Core::logger.AddLogString( LogLevels::ALL, "Used build name: " + which_build + " but it does not exist in builds list in config" );
			Display( "{fc}Unable to find build: {sc}" + which_build + "{fc}, exiting {br}" + UTF::CROSS + "{0}\n" );
			return Core::ReturnVar( 1 );
		}

		if( pconf.GetData().builds[ j ].type == "bin" ) {
			if( Project::BuildBinary( pconf.GetData(), j, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
		}
		else if( pconf.GetData().builds[ j ].type == "test" ) {
			if( Project::BuildTest( pconf.GetData(), j, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
		}
		else if( pconf.GetData().builds[ j ].type == "lib" ) {
			if( Project::BuildLibrary( pconf.GetData(), j, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
		}
	}
	else { // User didnt specify a build therefore build everything
		bool disp_cmds_only = args.size() > 3 && args[ 3 ] == "--cmds_only";

		for( size_t i = 0; i < pconf.GetData().builds.size(); ++i ) {
			if( pconf.GetData().builds[ i ].type == "bin" ) {
				if( Project::BuildBinary( pconf.GetData(), i, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
			}
			else if( pconf.GetData().builds[ i ].type == "test" ) {
				if( Project::BuildTest( pconf.GetData(), i, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
			}
			else if( pconf.GetData().builds[ i ].type == "lib" ) {
				if( Project::BuildLibrary( pconf.GetData(), i, disp_cmds_only ) != 0 ) return Core::ReturnVar( 1 );
			}
		}
	}

	return Core::ReturnVar( 0 );
}

int Project::Add( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Add" );

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnVar( 1 );
	}

	if( args.size() < 4 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No parameter ( source / header / build / lib ) specified to set" );
		Display( "{fc}No parameter specified to set. Use a parameter{0}: {sc}inc{0} / {sc}src{0} / {sc}build{0} / {sc}lib{0} ...\n" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;

	pconf.LoadFile( Env::CCP4M_PROJECT_CONFIG_FILE );
	Display( "\n" );

	if( pconf.GetData().name.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name! Unable to continue." );
		Display( "{fc}No project name exists in the configuration file. Unable to continue{0}\n" );
		return Core::ReturnVar( 1 );
	}

	if( args[ 3 ] == "src" || args[ 3 ] == "inc" || args[ 3 ] == "test" ) return Core::ReturnVar( Project::AddFiles( pconf, args ) );
	if( args[ 3 ] == "build" || args[ 3 ] == "lib" ) return Core::ReturnVar( Project::AddProjectInfo( pconf, args ) );

	Core::logger.AddLogString( LogLevels::ALL, "Invalid parameter " + args[ 3 ] + " specified to set" );
	Display( "{fc}Invalid parameter {sc}" + args[ 3 ] + "{fc} provided, correct possible parameters are{0}: {sc}inc{0} / {sc}src{0} / {sc}build{0} / {sc}lib{0}\n" );
	return Core::ReturnVar( 1 );
}

int Project::Set( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Set" );

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnVar( 1 );
	}

	if( args.size() < 4 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No parameter specified to set" );
		Display( "{fc}No parameter specified to set. Use a parameter like{0}: {sc}name{0} / {sc}compile_flags{0} / {sc}version {fc}and so on{0} ...\n" );
		return Core::ReturnVar( 1 );
	}

	if( args.size() < 5 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No value to the parameter specified" );
		Display( "{fc}No value to the parameter specified. Format is{0}: {sc}" + args[ 0 ] + " project set < parameter > < value >{0}\n" );
		Display( "{tc}\tNote{0}: {sc}Use double quotes to enclose a space separated string in the value field{0}\n" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;

	pconf.LoadFile( Env::CCP4M_PROJECT_CONFIG_FILE, false );

	if( pconf.GetData().name.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name! Unable to continue." );
		Display( "{fc}No project name exists in the configuration file. Unable to continue{0}\n" );
		return Core::ReturnVar( 1 );
	}

	std::string old_var;

	if( args[ 3 ] == "name" ) {
		old_var = pconf.GetData().name;
		pconf.GetData().name = args[ 4 ];
	}
	else if( args[ 3 ] == "version" ) {
		old_var = pconf.GetData().version;
		pconf.GetData().version = args[ 4 ];
	}
	else if( args[ 3 ] == "std" ) {
		old_var = pconf.GetData().std;
		pconf.GetData().std = args[ 4 ];
	}
	else if( args[ 3 ] == "compile_flags" ) {
		old_var = pconf.GetData().compile_flags;
		pconf.GetData().compile_flags = args[ 4 ];
	}
	else if( args[ 3 ] == "license" ) {
		if( std::find( License::LICENSES.begin(), License::LICENSES.end(), args[ 4 ] ) == License::LICENSES.end() ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unknown license specified: " + args[ 4 ] );
			Display( "{fc}Unknown license {r}" + args[ 4 ] + "{fc} specified, possible options are{0}:\n" );
			for( auto & lic : License::LICENSES )
				Display( "{sc}\t" + lic + "{0}\n" );
			return Core::ReturnVar( 1 );
		}

		if( !License::UpdateProjectLicenseFile( args[ 4 ] ) ) return Core::ReturnVar( 1 );

		if( !License::UpdateProjectSourceFiles( pconf.GetData().license, args[ 4 ], pconf.GetData().lang ) ) return Core::ReturnVar( 1 );

		old_var = pconf.GetData().license;
		pconf.GetData().license = args[ 4 ];
	}
	else {
		Core::logger.AddLogString( LogLevels::ALL, "Unknown parameter specified: " + args[ 3 ] );
		Display( "{fc}Unknown parameter {r}" + args[ 3 ] + "{fc} specified{0}\n" );
		return Core::ReturnVar( 1 );
	}

	pconf.SaveFile( Env::CCP4M_PROJECT_CONFIG_FILE );

	Core::logger.AddLogString( LogLevels::ALL, "Altered option: " + args[ 3 ] + " from: " + old_var + " to: " + args[ 4 ] );
	Display( "\n{fc}Alteration successful{0}\n" );

	return Core::ReturnVar( 0 );
}

int Project::Run( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Run" );

	if( args.size() < 4 ) {
		Display( "{fc}Build to execute is not specified{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Build to run is not specified" );
		return Core::ReturnVar( 1 );
	}

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;
	pconf.LoadFile( "ccp4m.yaml", false, true );

	int which = -1;

	for( size_t i = 0; i < pconf.GetData().builds.size(); ++i ) {
		if( pconf.GetData().builds[ i ].name == args[ 3 ] ) {
			which = i;
			break;
		}
	}

	if( which == -1 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Used build name: " + args[ 3 ] + " but it does not exist in builds list in config" );
		Display( "{fc}Unable to find build: {sc}" + args[ 3 ] + "{fc}. Exiting. {br}" + UTF::CROSS + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	auto & build = pconf.GetData().builds[ which ];

	if( build.type != "bin" ) {
		Display( "{fc}The specified build {sc}" + build.name + "{fc} is not a binary, it is a {sc}" + build.type + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Build: " + build.name + " is not binary, it is: " + build.type );
		return Core::ReturnVar( 1 );
	}

	if( !FS::LocExists( "bin/" + build.name ) ) {
		Display( "{fc}The specified build {sc}" + build.name + "{fc} is not already built, build it first using{0}: {tc}" + args[ 0 ] + " project build{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Build: " + build.name + " is not already built, it must be first built using " + args[ 0 ] + " project build" );
		return Core::ReturnVar( 1 );
	}

	std::string newargs;
	for( size_t i = 4; i < args.size(); ++i ) newargs += args[ i ] + " ";

	std::string pre_exec = build.pre_exec.empty() ? "" : ( build.pre_exec + " " );

	return Core::ReturnVar( std::system( ( pre_exec + "bin/" + build.name + " " + newargs ).c_str() ) );
}

int Project::Test( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Test" );

	if( !FS::LocExists( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		std::string currdir = Env::GetCurrentDir();
		Core::logger.AddLogString( LogLevels::ALL, "No project configuration in directory: " + currdir );
		Display( "{fc}Project configuration file: {sc}" + Env::CCP4M_PROJECT_CONFIG_FILE + "{fc} does not exist in this directory.{0}" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;
	pconf.LoadFile( "ccp4m.yaml", false, true );

	Display( "{fc}\nPerforming tests{0} ...\n\n" );

	int succ = 0, fail = 0;
	for( auto & build : pconf.GetData().builds ) {
		if( build.type != "test" ) continue;

		if( !FS::LocExists( "testbin/" + build.name ) ) {
			Display( "{fc}The test case {sc}" + build.name + "{fc} is not already built, build it first using{0}: {tc}" + args[ 0 ] + " project build{0}\n" );
			Core::logger.AddLogString( LogLevels::ALL, "Test case: " + build.name + " is not already built, it must be first built using " + args[ 0 ] + " project build" );
			return Core::ReturnVar( 1 );
		}

		std::string newargs;
		for( size_t i = 4; i < args.size(); ++i ) newargs += args[ i ] + " ";

		std::string pre_exec = build.pre_exec.empty() ? "" : ( build.pre_exec + " " );

		Display( "{fc}Testing case{0}: {sc}" + build.name + "{0} ... " );
		int ret = std::system( ( pre_exec + "testbin/" + build.name + " " + newargs ).c_str() );
		if( ret != 0 ) {
			Display( "{r}ERR{0}\n" );
			fail++;
		}
		else {
			Display( "{g}OK{0}\n" );
			succ++;
		}
	}

	if( !succ && !fail ) {
		Display( "{fc}There are no test cases in the project builds\n" );
		return Core::ReturnVar( 1 );
	}
	else {
		Display( "\n{fc}Tests completed{0}, {fc}success{0}: {g}" + std::to_string( succ ) + "{0}, {fc}failed{0}: {r}" + std::to_string( fail ) + "{0}\n" );
	}
	return Core::ReturnVar( 0 );
}

int Project::Clean()
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "Clean" );

	Display( "{fc}Cleaning project directory {0}...\n\n" );
	Core::logger.AddLogString( LogLevels::ALL, "Cleaning project build directory" );

	Display( "{sc}=> {fc}Removing binary directory {0}... " );
	Core::logger.AddLogString( LogLevels::ALL, "Removing binary directory" );

	if( FS::DeleteDir( "bin" ) ) {
		Display( "{g}Success{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Successful" );
	}
	else {
		Display( "{r}Failed{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed deleting directory: bin" );
		return Core::ReturnVar( 1 );
	}

	Display( "{sc}=> {fc}Removing testbin directory {0}... " );
	Core::logger.AddLogString( LogLevels::ALL, "Removing testbin directory" );

	if( FS::DeleteDir( "testbin" ) ) {
		Display( "{g}Success{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Successful" );
	}
	else {
		Display( "{r}Failed{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed deleting directory: testbin" );
		return Core::ReturnVar( 1 );
	}

	Display( "{sc}=> {fc}Removing library directory {0}... " );
	Core::logger.AddLogString( LogLevels::ALL, "Removing library directory" );

	if( FS::DeleteDir( "lib" ) ) {
		Display( "{g}Success{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Successful" );
	}
	else {
		Display( "{r}Failed{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed deleting directory: lib" );
		return Core::ReturnVar( 1 );
	}

	Display( "{sc}=> {fc}Removing buildfiles directory {0}... " );
	Core::logger.AddLogString( LogLevels::ALL, "Removing buildfiles directory" );

	if( FS::DeleteDir( "buildfiles" ) ) {
		Display( "{g}Success{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Successful" );
	}
	else {
		Display( "{r}Failed{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed deleting directory: buildfiles" );
		return Core::ReturnVar( 1 );
	}

	Display( "\n{fc}Successfully cleaned directory{0}\n" );
	Core::logger.AddLogString( LogLevels::ALL, "Successfully cleaned directory" );
	return Core::ReturnVar( 0 );
}
