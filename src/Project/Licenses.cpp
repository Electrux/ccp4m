#include <vector>
#include <string>
#include <curl/curl.h>

#include "../../include/Environment.hpp"
#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Network.hpp"
#include "../../include/Vars.hpp"
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

// This is used as LICENSES[ DEFAULT_LICENSE ]
const License::ID License::DEFAULT_LICENSE = License::ID::gnugpl3_0;

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
	Display( "{fc}Fetching copy of license: {sc}" + LICENSES[ id ] + " {0}... " );
	if( NW::DownloadFile( license_web_loc, license_sys_loc ) != 0 ) {
		Display( "\n{r}Failed to fetch a copy of license: {sc}" + LICENSES[ id ] + "{0}\n" );
		return Core::ReturnVar( std::string() );
	}
	Display( "\n" );

	std::string license_str = FS::ReadFile( license_sys_loc );

	if( license_str.empty() ) {
		Display( "\n{r}Failed to read license from file: {sc}" + license_sys_loc + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to read license from file: " + license_sys_loc );

		return Core::ReturnVar( std::string() );
	}

	Vars::ReplaceVars( license_str, false );

	return Core::ReturnVar( license_str );
}

std::string License::FetchLicenseForFile( const License::ID & id )
{
	Core::logger.AddLogSection( "License" );
	Core::logger.AddLogSection( "FetchLicenseForFile" );

	if( id >= LICENSES.size() || id < 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Attempted to fetch an invalid license ID: " + std::to_string( id ) + ", min ID is 0 and max ID is: " + std::to_string( LICENSES.size() ) );
		return Core::ReturnVar( "" );
	}

	std::string license_web_loc = Env::LICENSE_URL + LICENSES[ id ] + "_mini.txt";
	std::string license_sys_loc = Env::CCP4M_LICENSE_DIR + "/" + LICENSES[ id ] + "_mini.txt";

	// check if license file already exists
	Display( "{fc}Fetching copy of license: {sc}" + LICENSES[ id ] + " {0}... " );
	if( NW::DownloadFile( license_web_loc, license_sys_loc ) != 0 ) {
		Display( "\n{r}Failed to fetch a copy of license: {sc}" + LICENSES[ id ] + "{0}\n" );
		return Core::ReturnVar( "" );
	}
	Display( "\n" );

	std::string license_str = FS::ReadFile( license_sys_loc );

	if( license_str.empty() ) {
		Display( "\n{r}Failed to read license from file: {sc}" + license_sys_loc + "{0}\n" );
		Core::logger.AddLogString( LogLevels::ALL, "Failed to read license from file: " + license_sys_loc );

		return Core::ReturnVar( "" );
	}

	Vars::ReplaceVars( license_str, false );

	return Core::ReturnVar( license_str );
}