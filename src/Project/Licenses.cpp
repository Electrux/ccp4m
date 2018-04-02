#include <vector>
#include <string>
#include <curl/curl.h>

#include "../../include/Environment.hpp"
#include "../../include/Core.hpp"

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
		return Core::ReturnString( "" );
	}

	// check if license file already exists
	// download it if not
	// fetch and format contents
	// return formatted string
}

std::string License::FetchLicenseForFile( const License::ID & id );