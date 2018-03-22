#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Environment.hpp"

#include "../../include/Project/Config.hpp"

void AddLibrary( const Library & lib );
void AddBuild( const Build & build );

ProjectData & GetData();

bool ProjectConfig::GenerateDefaultConfig()
{
	YAML::Node conf = YAML::LoadFile( Env::CCP4M_CONFIG_FILE );

	if( !conf[ "name" ] || !conf[ "email" ] ) {
		Display( "{r}Unable to fetch name and email from system config.{0}" );
		return false;
	}

	pdata.author.name = conf[ "name" ].as< std::string >();
	pdata.author.email = conf[ "email" ].as< std::string >();

	pdata.name = "DefaultProject";
	pdata.version = "0.1";
	pdata.lang = "c++";
	pdata.std = "14";
	pdata.type = "bin";
	pdata.compile_flags = "-O2";

	// Will be set when the build command is called
	pdata.build_date = "None";

	Build build;
	build.name = "DefaultProject";
	build.main_src = "src/main.cpp";
}

bool ProjectConfig::LoadFile( const std::string & file )
{
	if( !FS::LocExists( file ) )
		return false;

	YAML::Node conf = YAML::LoadFile( file );

	pdata.name = conf[ "name" ].as< std::string >();
	pdata.version = conf[ "version" ].as< std::string >();
	pdata.lang = conf[ "lang" ].as< std::string >();
	pdata.std = conf[ "std" ].as< std::string >();
	pdata.type = conf[ "type" ].as< std::string >();
	pdata.compile_flags = conf[ "compile_flags" ].as< std::string >();
	pdata.build_date = conf[ "build_date" ].as< std::string >();

	pdata.author.name = conf[ "author" ][ "name" ].as< std::string >();
	pdata.author.email = conf[ "author" ][ "email" ].as< std::string >();

	for( auto libdata : conf[ "libs" ] ) {
		Library lib;

		lib.name = libdata[ "name" ].as< std::string >();
		lib.version = libdata[ "version" ].as< std::string >();
		lib.inc_flags = libdata[ "inc_flags" ].as< std::string >();
		lib.lib_flags = libdata[ "lib_flags" ].as< std::string >();

		pdata.libs.push_back( lib );
	}

	for( auto builddata : conf[ "builds" ] ) {
		Build build;

		build.name = builddata[ "name" ].as< std::string >();
		build.main_src = builddata[ "main_src" ].as< std::string >();

		for( auto othersrcs : conf[ "builds" ][ "other_src" ] ) {
			build.srcs.push_back( othersrcs.as< std::string >() );
		}

		pdata.builds.push_back( build );
	}
}

bool SaveFile( const std::string & file );

void ProjectConfig::DisplayAll()
{
	Display( "{by}------------------------------------------------\n\n" );
	Display( "{bm}Name: {bg}" + pdata.name + "\n" );
	Display( "{bm}Version: {bg}" + pdata.version + "\n" );
	Display( "{bm}Lang: {bg}" + pdata.lang + "\n" );
	Display( "{bm}Std: {bg}" + pdata.std + "\n" );
	Display( "{bm}Type: {bg}" + pdata.type + "\n" );
	Display( "{bm}Compile_Flags: {bg}" + pdata.compile_flags + "\n\n" );

	Display( "{by}Libs:\n" );

	for( auto lib : pdata.libs ) {
		Display( "\t{bm}Name: {bg}" + lib.name + "\n" );
		Display( "\t{bm}Version: {bg}" + lib.version + "\n" );
		Display( "\t{bm}Inc_Flags: {bg}" + lib.inc_flags + "\n" );
		Display( "\t{bm}Lib_Flags: {bg}" + lib.lib_flags + "\n\n" );
	}

	Display( "\n" );

	Display( "{by}Builds:\n" );

	for( auto build : pdata.builds ) {
		Display( "\t{bm}Name: {bg}" + build.name + "\n" );
		Display( "\t{bm}Version: {bg}" + build.main_src + "\n" );
		for( auto s : build.srcs ) {
			Display( "\t{bg}" + s + "\n" );
		}
	}

	Display( "\n{by}--------------------------------------------------{0}\n\n");
}