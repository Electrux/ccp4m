#include <string>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

#include "../include/Environment.hpp"
#include "../include/Core.hpp"

#include "../include/FSFuncs.hpp"

bool FS::LocExists( const std::string & loc )
{
	if( loc.empty() )
		return true;

	struct stat info;

	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "LocExists" );

	if( stat( loc.c_str(), & info ) == 0 || lstat( loc.c_str(), & info ) == 0 ) {
		Core::logger.AddLogString( LogLevels::ALL, "Location: " + loc + " exists" );
		return Core::ReturnBool( true );
	}
 
	Core::logger.AddLogString( LogLevels::ALL, "Location: " + loc + " does not exist" );
	return Core::ReturnBool( false );
}

bool FS::CreateDir( const std::string & dir )
{
	if( LocExists( dir ) )
		return true;

	int ctr = 0;
	std::string fdir;

	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "CreateDir" );

	for( auto ch : dir ) {
		if( ch == '~' ) {
			fdir += Env::GetEnvVar( "HOME" );
		}
		else {
			fdir += ch;
		}
		if( ch == '/' && !LocExists( fdir ) ) {
			if( mkdir( fdir.c_str(), 0755 ) != 0 ) {
				Core::logger.AddLogString( LogLevels::ALL, "Unable to create directory: " + fdir );
				return Core::ReturnBool( false );
			}
		}
	}

	if( !LocExists( fdir ) ) {
		if( mkdir( fdir.c_str(), 0755 ) != 0 ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to create final directory: " + fdir );
			return Core::ReturnBool( false );
		}
		Core::logger.AddLogString( LogLevels::ALL, "Final directory successfully created: " + fdir );
	}

	return Core::ReturnBool( true );
}

bool FS::CreateFile( const std::string & loc, const std::string & contents )
{
	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "CreateFile" );

	auto last_slash = loc.rfind( '/' );
	if( last_slash != std::string::npos ) {
		std::string dir = loc.substr( 0, last_slash );
		if( !LocExists( dir ) && !CreateDir( dir ) ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to access/create directory: " + dir + " for file: " + loc );
			return Core::ReturnBool( false );
		}
	}

	if( loc.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Filename is empty" );
		return Core::ReturnBool( false );
	}

	std::fstream file;
	file.open( loc, std::ios::out );

	if( !file ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create and open file: " + loc );
		return Core::ReturnBool( false );
	}

	if( !contents.empty() )
		file << contents;

	file.close();

	Core::logger.AddLogString( LogLevels::ALL, "Successfully created file: " + loc + " with contents spanning " + std::to_string( contents.size() ) + " bytes" );
	return Core::ReturnBool( true );
}

bool FS::CreateFileIfNotExists( const std::string & loc, const std::string & contents )
{
	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "CreateFileIfNotExists" );

	auto last_slash = loc.rfind( '/' );
	if( last_slash != std::string::npos ) {
		std::string dir = loc.substr( 0, last_slash );
		if( !LocExists( dir ) && !CreateDir( dir ) ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to access/create directory: " + dir + " for file: " + loc );
			return Core::ReturnBool( false );
		}
	}

	if( loc.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Filename is empty" );
		return Core::ReturnBool( false );
	}

	std::fstream file;
	file.open( loc, std::ios::out | std::ios::app );

	if( !file ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create and/or open file: " + loc );
		return Core::ReturnBool( false );
	}

	if( !contents.empty() )
		file << contents;

	file.close();

	Core::logger.AddLogString( LogLevels::ALL, "Successfully created file: " + loc + " with contents spanning " + std::to_string( contents.size() ) + " bytes" );
	return Core::ReturnBool( true );
}