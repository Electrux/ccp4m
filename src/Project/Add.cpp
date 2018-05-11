/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <vector>
#include <string>
#include <map>

#include "../../include/Core.hpp"
#include "../../include/Environment.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Vars.hpp"
#include "../../include/UTFChars.hpp"

#include "../../include/Project/Config.hpp"
#include "../../include/Project/CodeFileGenerator.hpp"

#include "../../include/Project/Add.hpp"

int Project::AddFiles( ProjectConfig & pconf, const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "AddFiles" );

	if( args.size() < 5 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No value to the parameter ( source / header ) specified" );
		Display( "{fc}No value to the parameter specified. Format is{0}: {sc}" + args[ 0 ] + " project set < parameter > < value > < optional: build_name >{0}\n" );
		Display( "{tc}\tNote{0}: {sc}Use double quotes to enclose a space separated string in the value field{0}, {r}although that is highly discouraged{0}\n" );
		return Core::ReturnVar( 1 );
	}

	std::string file;
	std::string ext;
	if( args[ 3 ] == "src" ) {
		ext = pconf.GetData().lang == "c++" ? ".cpp" : ".c";
		file = "src/";
	}
	else if( args[ 3 ] == "inc" ) {
		ext = pconf.GetData().lang == "c++" ? ".hpp" : ".h";
		file = "include/";
	}
	else if( args[ 3 ] == "test" ) {
		ext = pconf.GetData().lang == "c++" ? ".cpp" : ".c";
		file = "tests/";
	}
	file += args[ 4 ] + ext;

	if( FS::LocExists( file ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "File: " + file + " already exists" );
		Display( "{fc}File{0}: {r}" + file + "{fc} already exists{0}\n" );
		return Core::ReturnVar( 1 );
	}

	int which_build = -1;
	if( args.size() > 5 ) {
		for( size_t i = 0; i < pconf.GetData().builds.size(); ++i ) {
			if( pconf.GetData().builds[ i ].name == args[ 5 ] ) which_build = i;
		}

		if( which_build == -1 ) {
			Core::logger.AddLogString( LogLevels::ALL, "Used build name: " + args[ 5 ] + " but it does not exist in builds list in config" );
			Display( "{fc}Unable to find build: {sc}" + args[ 5 ] + "{fc}, exiting {br}" + UTF::CROSS + "{0}\n" );
			return Core::ReturnVar( 1 );
		}
	}

	if( args[ 3 ] == "src" ) return Core::ReturnVar( Project::GenerateSourceFile( pconf, args[ 4 ] + ext, which_build ) );
	if( args[ 3 ] == "test" ) return Core::ReturnVar( Project::GenerateTestFile( pconf, args[ 4 ] + ext ) );

	return Core::ReturnVar( Project::GenerateIncludeFile( pconf, args[ 4 ] + ext ) );
}

int Project::AddProjectInfo( ProjectConfig & pconf, const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "AddProjectInfo" );

	if( args.size() < 5 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name specified" );
		Display( "\n{fc}No project name specified{0}. {fc}Use the format{0}: {sc}" + args[ 0 ] + " project add {0}{{ {sc}build {0}/ {sc}lib {0}}{sc} {0}< {sc}name {0}>\n" );
		return Core::ReturnVar( 1 );
	}

	std::map< std::string, std::string > argmap;
	int res = 0;
	if( ( res = Str::FetchArgs( args, argmap, 5 ) ) != 0 ) return Core::ReturnVar( res );

	std::string name = args[ 4 ];

	if( args[ 3 ] == "lib" ) {
		Display( "{fc}Adding library{0}: {sc}" + name + "{0} ..." );

		for( auto & l : pconf.GetData().libs ) {
			if( l.name == name ) {
				Display( "{r} Failed\n" );
				Display( "{fc}Cannot add the library{0}: {r}" + name + "{0}, {fc}it already exists in configuration{0}\n" );
				return Core::ReturnVar( 1 );
			}
		}

		Config::Library lib;

		lib.name = name;
		lib.version = argmap.find( "version" ) != argmap.end() ? argmap[ "version" ] : "0";
		lib.inc_flags = argmap.find( "inc_flags" ) != argmap.end() ? argmap[ "inc_flags" ] : "";
		lib.lib_flags = argmap.find( "lib_flags" ) != argmap.end() ? argmap[ "lib_flags" ] : "";

		pconf.AddLibrary( lib );
	}

	if( args[ 3 ] == "build" ) {
		Display( "{fc}Adding build{0}: {sc}" + name + "{0} ..." );

		for( auto & b : pconf.GetData().builds ) {
			if( b.name == name ) {
				Display( "{r} Failed\n" );
				Display( "{fc}Cannot add the build{0}: {r}" + name + "{0}, {fc}it already exists in configuration{0}\n" );
				return Core::ReturnVar( 1 );
			}
		}

		Config::Build build;

		build.name = name;
		build.type = argmap.find( "type" ) != argmap.end() ? argmap[ "type" ] : "bin";

		if( build.type == "lib" ) build.build_type = argmap.find( "build_type" ) != argmap.end() ? argmap[ "build_type" ] : "static";

		std::string default_main = pconf.GetData().lang == "c++" ? "src/main.cpp" : "src/main.c";

		build.main_src = argmap.find( "main_src" ) != argmap.end() ? argmap[ "main_src" ] : default_main;

		if( argmap.find( "exclude" ) != argmap.end() ) build.exclude = Str::Delimit( argmap[ "exclude" ], ',' );
		if( argmap.find( "srcs" ) != argmap.end() ) build.srcs = Str::Delimit( argmap[ "srcs" ], ',' );

		pconf.AddBuild( build );
	}
	Display( "{g} Success\n\n" );

	Display( "{fc}Updating project configuration file {0}..." );
	if( !pconf.SaveFile( Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		Display( "{r} Failed\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success\n" );
	return Core::ReturnVar( 0 );
}
