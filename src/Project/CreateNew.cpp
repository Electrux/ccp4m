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

#include "../../include/Project/Config.hpp"
#include "../../include/Project/Licenses.hpp"

#include "../../include/Project/CreateNew.hpp"

// FetchArgs must have from = 4 ( after ccp4m project new < name > ... )
int Project::New( const std::vector< std::string > & args )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "New" );

	if( args.size() < 4 ) {
		Core::logger.AddLogString( LogLevels::ALL, "No project name specified" );
		Display( "\n{fc}No project name specified{0}. {fc}Use the format{0}: {sc}" + args[ 0 ] + " project new < name >{0}\n" );
		return Core::ReturnVar( 1 );
	}

	std::map< std::string, std::string > argmap;
	int res = 0;
	if( ( res = Str::FetchArgs( args, argmap, 4 ) ) != 0 ) {
		return Core::ReturnVar( res );
	}

	std::string project_dir;

	if( argmap.find( "dir" ) != argmap.end() ) {
		project_dir = argmap[ "dir" ];
		Env::ReplaceTildeWithHome( project_dir );
		if( * ( project_dir.end() - 1 ) != '/' )
			project_dir += "/";
	}

	if( FS::LocExists( project_dir + Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "Directory: " + Env::GetCurrentDir() + " already contains a project configuration file" );
		ProjectConfig pconf;
		pconf.LoadFile( Env::CCP4M_PROJECT_CONFIG_FILE, false );
		Display( "\n{fc}This directory is already home to a project{0}: {r}" + pconf.GetData().name + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	ProjectConfig pconf;
	pconf.GenerateDefaultConfig();

	Display( "\n{br}Project information:\n\n" );

	Core::logger.AddLogString( LogLevels::ALL, "Project name: " + args[ 3 ] );
	pconf.GetData().name = args[ 3 ];

	if( argmap.find( "lang" ) != argmap.end() ) {
		argmap[ "lang" ] = Str::ToLower( argmap[ "lang" ] );
		if( argmap[ "lang" ] == "cpp" )
			argmap[ "lang" ] = "c++";
		Core::logger.AddLogString( LogLevels::ALL, "Project language: " + argmap[ "lang" ] );
		pconf.GetData().lang = argmap[ "lang" ];
	}

	if( argmap.find( "version" ) != argmap.end() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Project version: " + argmap[ "version" ] );
		pconf.GetData().version = argmap[ "version" ];
	}

	if( argmap.find( "std" ) != argmap.end() ) {
		if( argmap[ "std" ].find_first_not_of( "0123456789" ) != std::string::npos ) {
			Display( "{fc}Paramter{0}: {sc}std {fc}can only have a value of numeric type{0}, {fc}provided value{0}: {r}" + argmap[ "std" ] + "{0}\n" );
			Display( "{fc}Ignoring it {0}...\n" );
		}
		else {
			Core::logger.AddLogString( LogLevels::ALL, "Project standard: " + argmap[ "std" ] );
			pconf.GetData().std = argmap[ "std" ];
		}
	}

	if( argmap.find( "license" ) != argmap.end() ) {
		if( std::find( License::LICENSES.begin(), License::LICENSES.end(), argmap[ "license" ] ) == License::LICENSES.end() ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unknown license specified: " + args[ 4 ] );
			Display( "{fc}Unknown license {r}" + argmap[ "license" ] + "{fc} specified, possible options are{0}:\n" );
			for( auto & lic : License::LICENSES )
				Display( "{sc}\t" + lic + "{0}\n" );
			return Core::ReturnVar( 1 );
		}
		Core::logger.AddLogString( LogLevels::ALL, "Project license: " + argmap[ "license" ] );
		pconf.GetData().license = argmap[ "license" ];
	}

	if( argmap.find( "author.name" ) != argmap.end() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Project author name: " + argmap[ "author.name" ] );
		pconf.GetData().author.name = argmap[ "author.name" ];
	}

	if( argmap.find( "author.email" ) != argmap.end() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Project author email: " + argmap[ "author.email" ] );
		pconf.GetData().author.email = argmap[ "author.email" ];
	}

	auto & build = pconf.GetData().builds[ 0 ];

	if( argmap.find( "build.name" ) != argmap.end() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Project build name: " + argmap[ "build.name" ] );
		build.name = argmap[ "build.name" ];
	}
	if( argmap.find( "build.type" ) != argmap.end() ) {
		if( argmap[ "build.type" ] != "bin" && argmap[ "build.type" ] != "lib" && argmap[ "build.type" ] != "test" ) {
			Display( "{fc}Paramter{0}: {sc}build{0}.{sc}type {fc}can only have a value either {sc}bin {fc}or {sc}lib{0}, {fc}provided value{0}: {r}" + argmap[ "std" ] + "{0}\n" );
			Display( "{fc}Ignoring it {0}...\n\n" );
		}
		else {
			Core::logger.AddLogString( LogLevels::ALL, "Project build type: " + argmap[ "build.type" ] );
			build.type = argmap[ "build.type" ];
		}
	}
	if( argmap.find( "build.main_src" ) != argmap.end() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Project build main source: " + argmap[ "build.main_src" ] );
		build.main_src = argmap[ "build.main_src" ];
	}

	if( pconf.GetData().lang == "c" ) {
		pconf.GetData().builds[ 0 ].main_src = "src/main.c";
		pconf.GetData().builds[ 0 ].srcs[ 0 ] = "src/(.*).c";
		if( argmap.find( "std" ) == argmap.end() )
			pconf.GetData().std = "11";
	}

	pconf.DisplayAll( project_dir );

	Display( "\n" );

	Display( "{fc}Creating project directories{0} ...\n\n" );
	if( !project_dir.empty() ) {
		Display( "{sc}=> {fc}" + project_dir + " {0}..." );
		if( !FS::CreateDir( project_dir ) ) {
			Display( "{r} Failed{0}\n" );
			Display( "{sc}=> {fc}Unable to create project directory{0}: {r}" + project_dir + " {0}, {fc}will not continue{0}\n" );
			return Core::ReturnVar( 1 );
		}
		Display( "{g} Success\n" );
	}
	Display( "{sc}=> {fc}" + project_dir + "include {0}..." );
	if( !FS::CreateDir( project_dir + "include" ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{sc}=> {fc}Unable to create project directory{0}: {r}" + project_dir + "include {0}, {fc}will not continue{0}\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success\n" );

	Display( "{sc}=> {fc}" + project_dir + "src {0}..." );
	if( !FS::CreateDir( project_dir + "src" ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{sc}=> {fc}Unable to create project directory{0}: {r}" + project_dir + "src {0}, {fc}will not continue{0}\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success\n\n" );

	Display( "{sc}=> {fc}" + project_dir + "tests {0}..." );
	if( !FS::CreateDir( project_dir + "tests" ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{sc}=> {fc}Unable to create project directory{0}: {r}" + project_dir + "tests {0}, {fc}will not continue{0}\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success\n\n" );

	auto v = Vars::GetSingleton();
	v->AddVar( "license", License::FetchLicenseFormalName( pconf.GetData().license ) );
	v->AddVar( "name", pconf.GetData().name );
	v->AddVar( "author", pconf.GetData().author.name );

	std::string && license_info = License::FetchLicense( pconf.GetData().license );
	std::string && license_info_mini = "/*\n" + License::FetchLicenseForFile( pconf.GetData().license ) + "\n*/";

	Display( "\n" );

	std::string main_src = pconf.GetData().builds[ 0 ].main_src;

	Display( "{fc}Creating build source file{0}: {sc}" + project_dir + main_src + " {0}..." );
	if( !FS::CreateFile( project_dir + main_src, license_info_mini + "\n\n" + pconf.GetDefaultMainFile() ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{fc}Unable to create source file{0}, {fc}see log for error{0}\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success{0}\n\n" );

	Core::logger.AddLogString( LogLevels::ALL, "Saving new configuration file" );
	Display( "{fc}Creating configuration file{0}: {sc}" + project_dir + Env::CCP4M_PROJECT_CONFIG_FILE + " {0}..." );
	if( !pconf.SaveFile( project_dir + Env::CCP4M_PROJECT_CONFIG_FILE ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{fc}Unable to save configuration file{0}, {fc}see log for error{0}\n" );
		return Core::ReturnVar( 1 );
	}
	Display( "{g} Success{0}\n\n" );

	Display( "{fc}Creating license file{0}: {sc}" + project_dir + "LICENSE {0}..." );
	if( !FS::CreateFile( project_dir + "LICENSE", license_info ) ) {
		Display( "{r} Failed{0}\n" );
		Display( "{fc}Unable to save license file{0}, {fc}see log for error{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "{g} Success{0}\n\n" );
	Display( "{fc}Project created successfully{0}\n" );
	return Core::ReturnVar( 0 );
}
