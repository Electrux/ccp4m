#include <string>
#include <regex>

#include "../../include/Core.hpp"
#include "../../include/Environment.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/Project/Config.hpp"
#include "../../include/Project/Licenses.hpp"

#include "../../include/Project/CodeFileGenerator.hpp"

int Project::GenerateIncludeFile( ProjectConfig & pconf, const std::string & file, const int which_build )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "GenerateIncludeFile" );

	std::string final_file = "include/" + file;

	std::string && license_str = License::FetchLicenseForFile( pconf.GetData().license );

	std::string caps_file = Str::ToUpper( file );
	std::replace( caps_file.begin(), caps_file.end(), '/', '_' );
	std::replace( caps_file.begin(), caps_file.end(), '.', '_' );
	std::replace( caps_file.begin(), caps_file.end(), ' ', '_' );

	std::string inc_data = "/*\n" + license_str + "*/\n\n#ifndef " + caps_file + "\n#define " + caps_file + "\n\n\n\n#endif //" + caps_file;

	if( !FS::CreateFile( final_file, inc_data ) ) {
		Display( "{fc}Failed to create include file{0}: {r}" + final_file + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "{fc}Successfully created include file{0}: {g}" + final_file + "{0}\n" );
	return Core::ReturnVar( 0 );
}

int Project::GenerateSourceFile( ProjectConfig & pconf, const std::string & file, const int which_build )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "GenerateSourceFile" );

	std::string final_file = "src/" + file;

	if( which_build == -1 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Adding source file: " + final_file + " to all builds" );
		for( auto & b : pconf.GetData().builds ) {
			bool found_regex = false;

			for( auto & srcs : b.srcs ) {
				if( std::regex_match( final_file, std::regex( srcs ) ) )
					found_regex = true;
					break;
			}

			if( found_regex )
				continue;

			b.srcs.push_back( final_file );
			Core::logger.AddLogString( LogLevels::ALL, "Added source file: " + final_file + " to build: " + b.name );
		}
	}
	else {
		auto & b = pconf.GetData().builds[ which_build ];
		bool found_regex = false;

		Core::logger.AddLogString( LogLevels::ALL, "Adding source file: " + final_file + " to build: " + b.name );

		for( auto & srcs : b.srcs ) {
			if( std::regex_match( final_file, std::regex( srcs ) ) )
				found_regex = true;
				break;
		}

		if( !found_regex ) {
			b.srcs.push_back( final_file );
			Core::logger.AddLogString( LogLevels::ALL, "Added source file: " + final_file + " to build: " + b.name );
		}
	}

	pconf.SaveFile( Env::CCP4M_PROJECT_CONFIG_FILE );

	std::string && license_str = License::FetchLicenseForFile( pconf.GetData().license );

	std::string caps_file = Str::ToUpper( file );
	std::replace( caps_file.begin(), caps_file.end(), '/', '_' );
	std::replace( caps_file.begin(), caps_file.end(), '.', '_' );
	std::replace( caps_file.begin(), caps_file.end(), ' ', '_' );

	std::string src_data = "/*\n" + license_str + "*/\n\n";

	if( !FS::CreateFile( final_file, src_data ) ) {
		Display( "{fc}Failed to create source file{0}: {r}" + final_file + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "{fc}Successfully created source file{0}: {g}" + final_file + "{0}\n" );
	return Core::ReturnVar( 0 );
}