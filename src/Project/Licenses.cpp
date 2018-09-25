#include <vector>
#include <string>
#include <curl/curl.h>

#include "../../include/Environment.hpp"
#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Network.hpp"
#include "../../include/Vars.hpp"
#include "../../include/StringFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/Project/Licenses.hpp"

const std::vector< std::string > License::LICENSES = {
	"apache2_0",
	"gnugpl3_0",
	"mit",
	"bsd2",
	"bsd3",
	"eclipse2_0",
	"gnuafferogpl3_0",
	"gnulessergpl2_1",
	"gnulessergpl3_0",
	"mozilla",
	"unlicense",
};

const std::vector< std::string > License::LICENSES_FORMAL = {
	"Apache 2.0",
	"GNU GPL 3.0",
	"MIT",
	"BSD 2-Clause",
	"BSD 3-Clause",
	"Eclipse 2.0",
	"GNU Affero GPL 3.0",
	"GNU Lesser GPL 2.1",
	"GNU Lesser GPL 3.0",
	"Mozilla",
	"Unlicense",
};

// This is used as LICENSES[ DEFAULT_LICENSE ]
const License::ID License::DEFAULT_LICENSE = License::ID::bsd3;

std::string License::FetchLicense( const License::ID & id )
{
	Core::logger.AddLogSection( "License" );
	Core::logger.AddLogSection( "FetchLicense" );

	if( id >= LICENSES.size() || id < 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Attempted to fetch an invalid license ID: " + std::to_string( id ) + ", min ID is 0 and max ID is: " + std::to_string( LICENSES.size() ) );
		return Core::ReturnVar( std::string() );
	}

	std::string license_web_loc = Env::LICENSE_URL + LICENSES[ id ] + ".txt";
	std::string license_sys_loc = Env::CCP4M_LICENSE_DIR + "/" + LICENSES[ id ] + ".txt";

	// check if license file already exists
	Display( "{fc}Fetching copy of license{0}: {sc}" + LICENSES[ id ] + " {0}..." );
	if( NW::DownloadFile( license_web_loc, license_sys_loc ) != 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Failed to fetch a copy of license: " + LICENSES[ id ] );
		Display( "{r} Failed\n" );
		Display( "\n{r}Failed to fetch a copy of license{0}: {sc}" + LICENSES[ id ] + "{0}\n" );
		return Core::ReturnVar( std::string() );
	}
	Display( "{g} Success\n" );

	std::string license_str = FS::ReadFile( license_sys_loc );

	if( license_str.empty() ) {
		Display( "\n{r}Failed to read license from file{0}: {sc}" + license_sys_loc + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to read license from file: " + license_sys_loc );
		return Core::ReturnVar( std::string() );
	}

	Vars::ReplaceVars( license_str, false );
	return Core::ReturnVar( license_str );
}

std::string License::FetchLicense( const std::string & name )
{
	int loc = -1;

	for( size_t i = 0; i < LICENSES.size(); ++i ) {
		if( LICENSES[ i ] == name ) {
			loc = ( int )i;
			break;
		}
	}

	if( loc != -1 ) return FetchLicense( ( ID )loc );
	return "";
}

std::string License::FetchLicenseForFile( const License::ID & id )
{
	Core::logger.AddLogSection( "License" );
	Core::logger.AddLogSection( "FetchLicenseForFile" );

	if( id >= LICENSES.size() || id < 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Attempted to fetch an invalid license ID: " + std::to_string( id ) + ", min ID is 0 and max ID is: " + std::to_string( LICENSES.size() ) );
		return Core::ReturnVar( "" );
	}

	std::string license_web_loc = Env::LICENSE_URL + "mini_license_for_file.txt";
	std::string license_sys_loc = Env::CCP4M_LICENSE_DIR + "/mini_license_for_file.txt";

	// check if license file already exists
	Display( "{fc}Fetching copy of {sc}mini {fc}license{0}: {sc}" + LICENSES[ id ] + " {0}..." );
	if( NW::DownloadFile( license_web_loc, license_sys_loc ) != 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Failed to fetch a copy of mini license: " + LICENSES[ id ] );
		Display( "{r} Failed\n" );
		Display( "\n{r}Failed to fetch a copy of {sc}mini {fc}license{0}: {sc}" + LICENSES[ id ] + "{0}\n" );
		return Core::ReturnVar( "" );
	}
	Display( "{g} Success\n" );

	std::string license_str = FS::ReadFile( license_sys_loc, "\t" );

	if( license_str.empty() ) {
		Display( "\n{r}Failed to read license from file{0}: {sc}" + license_sys_loc + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to read license from file: " + license_sys_loc );
		return Core::ReturnVar( "" );
	}

	Vars::ReplaceVars( license_str, false );
	return Core::ReturnVar( license_str );
}

std::string License::FetchLicenseForFile( const std::string & name )
{
	for( size_t i = 0; i < LICENSES.size(); ++i ) {
		if( LICENSES[ i ] == name ) return FetchLicenseForFile( ( ID )i );
	}

	return "";
}

std::string License::FetchLicenseFormalName( const std::string & license )
{
	for( size_t i = 0; i < LICENSES.size(); ++i ) {
		if( LICENSES[ i ] == license ) return LICENSES_FORMAL[ i ];
	}

	return "";
}

bool License::UpdateProjectLicenseFile( const std::string & license, bool really_update )
{
	Core::logger.AddLogSection( "License" );
	Core::logger.AddLogSection( "UpdateProjectLicenseFile" );

	std::string && license_str = License::FetchLicense( license );
	if( license_str == "" ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to retrieve license file" );
		return Core::ReturnVar( false );
	}
	if( !FS::LocExists( "LICENSE" ) || ( really_update && FS::ReadFile( "LICENSE" ) != license_str ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "License file not found in project directory or is different than it should be. Creating a new one" );
		Display( "{fc}Updating {sc}LICENSE {fc}file {0}...\n" );
		if( !FS::CreateFile( "LICENSE", license_str ) ) return Core::ReturnVar( false );
	}

	return Core::ReturnVar( true );
}

bool License::UpdateProjectSourceFiles( const std::string & old_license, const std::string & new_license, const std::string & lang )
{
	Core::logger.AddLogSection( "License" );
	Core::logger.AddLogSection( "UpdateProjectSourceFiles" );

	std::string regex_ext = lang == "c++" ? "(h|hpp|cpp)" : "(h|c)";

	auto files = FS::GetFilesInDir( ".", std::regex( "(include|src)/(.*)." + regex_ext ) );

	int alter_count = 0;

	for( auto & f : files ) {
		auto && content = FS::ReadFileVector( f );
		if( content[ 0 ] != "\n" && content[ 0 ].find( "/*" ) == std::string::npos ) continue;

		bool found = false;

		std::string formalname_old = FetchLicenseFormalName( old_license );
		std::string formalname_new = FetchLicenseFormalName( new_license );

		Core::logger.AddLogString( LogLevels::ALL, "Altering license from: " + formalname_old + " to: " + formalname_new + " in file: " + f );

		for( size_t i = 0; i < 10; ++i ) {
			if( i >= content.size() ) break;

			if( content[ i ].find( formalname_old ) != std::string::npos ) {
				Core::logger.AddLogString( LogLevels::ALL, "Altering on line: " + std::to_string( i ) + " with contents: " + content[ i ] );
				Str::Replace( content[ i ], formalname_old, formalname_new );
				found = true;
			}
		}

		if( !found ) continue;

		if( !FS::CreateFileVectorContents( f, content ) ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to create updated file: " + f );
			Display( "{fc}Unable to alter license in file{0}: {r}" + f + "{0}\n" );
			return Core::ReturnVar( false );
		}
		++alter_count;
	}

	Core::logger.AddLogString( LogLevels::ALL, "Altered license in " + std::to_string( alter_count ) + " files" );
	return Core::ReturnVar( true );
}
