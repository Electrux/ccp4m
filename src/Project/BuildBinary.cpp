#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/UTFChars.hpp"
#include "../../include/ExecuteCommand.hpp"

#include "../../include/Project/Config.hpp"

#include "../../include/Project/BuildBinary.hpp"

int Project::BuildBinary( const ProjectData & data, const int data_i )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildBinary" );

	std::string main_src;
	std::vector< std::string > files;

	std::string caps_lang;
	std::string compiler;
	std::string inc_flags, lib_flags;

	GetVars( data, data_i, main_src, compiler, caps_lang, inc_flags, lib_flags, files );

	Core::logger.AddLogString( LogLevels::ALL, "Compiling " + std::to_string( files.size() ) + " sources with main_src as: " + main_src );

	if( !CreateSourceDirs( files ) )
		return Core::ReturnInt( 1 );

	Core::logger.AddLogString( LogLevels::ALL, "Building target: " + data.builds[ data_i ].name );

	int total_sources = files.size() + ( int )!main_src.empty();

	Display( "{fc}Building target {sc}" + data.builds[ data_i ].name + " {fc}with {sc}" + std::to_string( total_sources ) + " {fc}sources {0}...\n\n" );

	std::string build_files_str;

	int ctr = 1;
	for( auto src : files ) {
		int percent = ( ctr * 100 / total_sources );
		Display( "{tc}[" + std::to_string( percent ) + "%]\t{fc}Compiling " + caps_lang + " object: {sc}buildfiles/" + src + ".o {0}...\n" );

		build_files_str += "buildfiles/" + src + ".o ";
		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";
		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnInt( ret_val );
		}
		++ctr;
	}

	if( !main_src.empty() ) {
		int percent = ( ctr * 100 / total_sources );
		Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + caps_lang + " binary: {sc}buildfiles/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " " + lib_flags + " -g -o buildfiles/" + data.builds[ data_i ].name + " " + main_src + " " + build_files_str;
		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnInt( ret_val );
		}

		Display( "\n{fc}Moving {sc}buildfiles/" + data.builds[ data_i ].name + "{fc} to {sc}bin/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string cmd = "mv buildfiles/" + data.builds[ data_i ].name + " bin/" + data.builds[ data_i ].name;
		ret_val = Exec::ExecuteCommand( cmd, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnInt( ret_val );
		}
	}

	

	return Core::ReturnInt( 0 );
}

void Project::GetFlags( const ProjectData & data, std::string & inc_flags, std::string & lib_flags )
{
	inc_flags = "";
	lib_flags = "";
	for( auto lib : data.libs ) {
		if( !lib.inc_flags.empty() )
			inc_flags += lib.inc_flags + " ";
		if( !lib.lib_flags.empty() )
			lib_flags += lib.lib_flags + " ";
	}
}

bool Project::CreateSourceDirs( const std::vector< std::string > & srcs )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "CreateSourceDirs" );

	for( auto src : srcs ) {
		auto last_slash = src.rfind( '/' );
		if( last_slash != std::string::npos ) {
			std::string dir = "buildfiles/" + src.substr( 0, last_slash );
			if( !FS::LocExists( dir ) ) {
				if( !FS::CreateDir( dir, false ) ) {
					Core::logger.AddLogString( LogLevels::ALL, "Unable to access/create directory: " + dir + " for source file: " + src );
					return Core::ReturnBool( false );
				}
				Core::logger.AddLogString( LogLevels::ALL, "Created build directory: " + dir + " for source file: " + src );
			}
		}
	}

	return Core::ReturnBool( true );
}

void Project::GetVars( const ProjectData & data, int data_i, std::string & main_src, std::string & compiler, std::string & caps_lang,
		std::string & inc_flags, std::string & lib_flags, std::vector< std::string > & files )
{
	main_src = data.builds[ data_i ].main_src;

	for( auto src : data.builds[ data_i ].srcs ) {
		std::vector< std::string > tempfiles;
		if( main_src.empty() )
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ) );
		else
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ), { main_src } );
		files.insert( files.end(), tempfiles.begin(), tempfiles.end() );
	}

	caps_lang = data.lang == "c++" ? "CXX" : "C";

	if( data.lang == "c++" )
		Core::SetVarArch( compiler, { "g++", "clang++", "clang++" } );
	else
		Core::SetVarArch( compiler, { "gcc", "clang", "clang" } );

	GetFlags( data, inc_flags, lib_flags );
}