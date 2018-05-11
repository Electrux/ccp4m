#include <string>
#include <curl/curl.h>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/Network.hpp"

static size_t curl_write_func( void *ptr, size_t size, size_t nmemb, void *stream );
int progress_func( void* ptr, double totdl, double cdl, double totup, double cup );

int NW::DownloadFile( const std::string & url, const std::string & out_loc )
{
	Core::logger.AddLogSection( "NW" );
	Core::logger.AddLogSection( "DownloadFile" );

	if( out_loc.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Cannot download to an empty location" );
		return Core::ReturnVar( 1 );
	}

	if( FS::LocExists( out_loc ) && FS::ReadFile( out_loc ).size() > 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Location: " + out_loc + " already exists" );
		return Core::ReturnVar( 0 );
	}

	CURLcode ret;
	CURL * hnd;

	std::FILE * file;

	if( url.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Cannot download from an empty URL" );
		return Core::ReturnVar( -1 );
	}

	hnd = curl_easy_init();
	if( !hnd ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to initialize curl using curl_easy_init()" );
		return Core::ReturnVar( 1 );
	}

	curl_easy_setopt( hnd, CURLOPT_URL, url.c_str() );
	file = std::fopen( out_loc.c_str(), "w" );

	curl_easy_setopt( hnd, CURLOPT_WRITEDATA, file );
	curl_easy_setopt( hnd, CURLOPT_WRITEFUNCTION, curl_write_func );
	curl_easy_setopt( hnd, CURLOPT_NOPROGRESS, 0 );
	curl_easy_setopt( hnd, CURLOPT_PROGRESSFUNCTION, progress_func );

	curl_easy_setopt( hnd, CURLOPT_FAILONERROR, 1L );
	curl_easy_setopt( hnd, CURLOPT_USERAGENT, "curl/7.54.0" );
	curl_easy_setopt( hnd, CURLOPT_MAXREDIRS, 50L );
	curl_easy_setopt( hnd, CURLOPT_HTTP_VERSION, ( long )CURL_HTTP_VERSION_2TLS );
	curl_easy_setopt( hnd, CURLOPT_TCP_KEEPALIVE, 1L );
	curl_easy_setopt( hnd, CURLOPT_SERVER_RESPONSE_TIMEOUT, 10L );

	ret = curl_easy_perform( hnd );

	Core::logger.AddLogString( LogLevels::ALL, "Downloading url: " + url + " output: " + out_loc );

	std::fclose( file );

	curl_easy_cleanup( hnd );

	hnd = NULL;

	DisplayOneLiner( "" );

	if( ( int )ret != 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Failed to download from URL: " + url );
		Core::logger.AddLogString( LogLevels::ALL, "LibCurl Error:" + std::string( curl_easy_strerror( ret ) ) );
		return Core::ReturnVar( ( int )ret );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Successfully downloaded file: " + out_loc + "from URL: " + url );

	return Core::ReturnVar( ( int )ret );
}

static size_t curl_write_func( void *ptr, size_t size, size_t nmemb, void *stream )
{
	size_t written = fwrite( ptr, size, nmemb, ( FILE * )stream );
	return written;
}

int progress_func( void* ptr, double totdl, double cdl, double totup, double cup )
{
	if( totdl <= 0.0 ) return 0;

	double percentdown = ( cdl / totdl ) * 100;
	std::string percent = std::to_string( percentdown ) + "%";

	DisplayOneLiner( "{bg}[ {bc}" + percent + " {bg}]{0}" );
	return 0;
}
