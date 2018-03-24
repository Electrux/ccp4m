#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/UTFChars.hpp"

#include "../../include/Project/Config.hpp"

#include "../../include/Project/BuildBinary.hpp"

int Project::BuildBinary( const ProjectData & data, const int data_i )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildBinary" );

	std::string main_src = data.builds[ data_i ].main_src;

	std::vector< std::string > files;

	for( auto src : data.builds[ data_i ].srcs ) {
		std::vector< std::string > tempfiles;
		if( main_src.empty() )
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ) );
		else
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ), { main_src } );
		files.insert( files.end(), tempfiles.begin(), tempfiles.end() );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Compiling " + std::to_string( files.size() ) + " sources with main_src as: " + main_src );

	std::string caps_lang = data.lang == "c++" ? "CXX" : "C";

	int total_sources = files.size() + main_src.empty();

	Display( "{fc}Building target {sc}" + data.builds[ data_i ].name + " {fc}with {sc}" + std::to_string( total_sources ) + " {fc}sources {0}...\n\n" );

	Core::logger.AddLogString( LogLevels::ALL, "Building target: " + data.builds[ data_i ].name );

	std::string compiler;
	if( data.lang == "c++" )
		Core::SetVarArch( compiler, { "g++", "clang++", "clang++" } );
	else
		Core::SetVarArch( compiler, { "gcc", "clang", "clang" } );

	for( auto src : files ) {
		Display( "{fc}Compiling " + caps_lang + " object: {sc}buildfiles/" + src + ".o {0}...\n" );
	}

	return Core::ReturnInt( 0 );
}