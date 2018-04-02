#include <vector>
#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"

#include "../../include/Project/Config.hpp"

#include "../../include/Project/BuildCommon.hpp"


void Common::GetFlags( const ProjectData & data, std::string & inc_flags, std::string & lib_flags )
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

bool Common::CreateSourceDirs( const std::vector< std::string > & srcs )
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
					return Core::ReturnVar( false );
				}
				Core::logger.AddLogString( LogLevels::ALL, "Created build directory: " + dir + " for source file: " + src );
			}
		}
	}

	return Core::ReturnVar( true );
}

void Common::GetVars( const ProjectData & data, int data_i, std::string & main_src, std::string & compiler, std::string & caps_lang,
		std::string & inc_flags, std::string & lib_flags, std::vector< std::string > & files )
{
	main_src = data.builds[ data_i ].main_src;

	std::vector< std::string > excludefiles = data.builds[ data_i ].exclude;
	if( !main_src.empty() )
		excludefiles.push_back( main_src );

	for( auto src : data.builds[ data_i ].srcs ) {
		std::vector< std::string > tempfiles;
		if( excludefiles.empty() )
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ) );
		else
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ), excludefiles );
		files.insert( files.end(), tempfiles.begin(), tempfiles.end() );
	}

	caps_lang = data.lang == "c++" ? "CXX" : "C";

	if( data.lang == "c++" )
		Core::SetVarArch( compiler, { "g++", "clang++", "clang++" } );
	else
		Core::SetVarArch( compiler, { "gcc", "clang", "clang" } );

	GetFlags( data, inc_flags, lib_flags );
}