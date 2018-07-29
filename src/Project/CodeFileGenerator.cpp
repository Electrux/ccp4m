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

int Project::GenerateIncludeFile( ProjectConfig & pconf, const std::string & file )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "GenerateIncludeFile" );

	std::string final_file = "include/" + file;

	std::string && license_str = License::FetchLicenseForFile( pconf.GetData().license );

	std::string file_w_uscore = file;
	char first_letter = * file_w_uscore.begin();

	bool is_prev_caps = first_letter >= 'A' && first_letter <= 'Z';

	for( auto it = file_w_uscore.begin() + 1; it != file_w_uscore.end(); ++it ) {
		if( * it >= 'A' && * it <= 'Z' ) {
			if( !is_prev_caps && !std::ispunct( * ( it - 1 ) ) ) {
				it = file_w_uscore.insert( it, '_' );
				++it;
			}
			is_prev_caps = true;
			continue;
		}
		is_prev_caps = false;
	}

	std::string caps_file = Str::ToUpper( file_w_uscore );
	std::replace( caps_file.begin(), caps_file.end(), '/', '_' );
	std::replace( caps_file.begin(), caps_file.end(), '.', '_' );
	std::replace( caps_file.begin(), caps_file.end(), ' ', '_' );

	std::string inc_data = "/*\n" + license_str + "*/\n\n#ifndef " + caps_file + "\n#define " + caps_file + "\n\n\n\n#endif // " + caps_file;

	if( !FS::CreateFile( final_file, inc_data ) ) {
		Display( "\n{fc}Failed to create include file{0}: {r}" + final_file + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "\n{fc}Successfully created include file{0}: {g}" + final_file + "{0}\n" );
	return Core::ReturnVar( 0 );
}

int Project::GenerateSourceFile( ProjectConfig & pconf, const std::string & file, const int which_build )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "GenerateSourceFile" );

	std::string final_file = "src/" + file;

	std::string && license_str = License::FetchLicenseForFile( pconf.GetData().license );

	if( which_build == -1 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unspecified build name, adding source file: " + final_file + " to all builds" );
		Display( "\n{fc}Warning{0}: {sc}Unspecified build name, will add the source to all builds{0}\n" );
		for( auto & b : pconf.GetData().builds ) {
			bool found_regex = false;

			for( auto & srcs : b.srcs ) {
				if( std::regex_match( final_file, std::regex( srcs ) ) ) {
					found_regex = true;
					break;
				}
			}

			if( found_regex ) continue;

			b.srcs.push_back( final_file );
			Core::logger.AddLogString( LogLevels::ALL, "Added source file: " + final_file + " to build: " + b.name );
			Display( "{sc}=> {fc}Added source file to build{0}: {tc}" + b.name + "{0}\n" );
		}
	}
	else {
		auto & b = pconf.GetData().builds[ which_build ];
		bool found_regex = false;

		Core::logger.AddLogString( LogLevels::ALL, "Adding source file: " + final_file + " to build: " + b.name );

		for( auto & srcs : b.srcs ) {
			if( std::regex_match( final_file, std::regex( srcs ) ) ) {
				found_regex = true;
				break;
			}
		}

		if( !found_regex ) {
			b.srcs.push_back( final_file );
			Core::logger.AddLogString( LogLevels::ALL, "Added source file: " + final_file + " to build: " + b.name );
			Display( "{sc}=> {fc}Added source file to build{0}: {tc}" + b.name + "{0}\n" );
		}
	}

	pconf.SaveFile( Env::CCP4M_PROJECT_CONFIG_FILE );

	std::string src_data = "/*\n" + license_str + "*/\n\n";

	if( !FS::CreateFile( final_file, src_data ) ) {
		Display( "\n{fc}Failed to create source file{0}: {r}" + final_file + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Display( "\n{fc}Successfully created source file{0}: {g}" + final_file + "{0}\n" );
	return Core::ReturnVar( 0 );
}

int Project::GenerateTestFile( ProjectConfig & pconf, const std::string & file, const std::string & ext )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "GenerateTestFile" );

	std::string final_file = "tests/" + file + ext;

	std::string && license_str = License::FetchLicenseForFile( pconf.GetData().license );

	std::string src_data = "/*\n" + license_str + "*/\n\n";

	if( !FS::CreateFile( final_file, src_data ) ) {
		Display( "\n{fc}Failed to create source file{0}: {r}" + final_file + "{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Config::Build b;
	b.name = file;
	b.inc_flags = "";
	b.lib_flags = "";
	b.main_src = final_file;
	b.srcs.push_back( "src/(.*).cpp" );
	b.exclude.push_back( "src/main.cpp" );
	b.type = "test";
	pconf.AddBuild( b );
	pconf.SaveFile( Env::CCP4M_PROJECT_CONFIG_FILE );

	Display( "\n{fc}Successfully created test file{0}: {g}" + final_file + "{0}\n" );
	return Core::ReturnVar( 0 );
}
