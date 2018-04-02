#include <string>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <regex>
#include <sstream>
#include <streambuf>

#include "../include/Environment.hpp"
#include "../include/Core.hpp"
#include "../include/DisplayFuncs.hpp"

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
		return Core::ReturnVar( true );
	}
 
	Core::logger.AddLogString( LogLevels::ALL, "Location: " + loc + " does not exist" );
	return Core::ReturnVar( false );
}

bool FS::CreateDir( const std::string & dir, bool check_exists )
{
	if( check_exists && LocExists( dir ) )
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
				return Core::ReturnVar( false );
			}
		}
	}

	if( !LocExists( fdir ) ) {
		if( mkdir( fdir.c_str(), 0755 ) != 0 ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to create final directory: " + fdir );
			return Core::ReturnVar( false );
		}
		Core::logger.AddLogString( LogLevels::ALL, "Final directory successfully created: " + fdir );
	}

	return Core::ReturnVar( true );
}

bool FS::CreateFileDir( const std::string & file )
{
	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "CreateFileDir" );

	if( file.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Filename is empty" );
		return Core::ReturnVar( false );
	}

	auto last_slash = file.rfind( '/' );
	if( last_slash != std::string::npos ) {
		std::string dir = file.substr( 0, last_slash );
		if( !LocExists( dir ) && !CreateDir( dir ) ) {
			Core::logger.AddLogString( LogLevels::ALL, "Unable to access/create directory: " + dir + " for file: " + file );
			return Core::ReturnVar( false );
		}
	}

	return Core::ReturnVar( true );
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
			return Core::ReturnVar( false );
		}
	}

	if( loc.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Filename is empty" );
		return Core::ReturnVar( false );
	}

	std::fstream file;
	file.open( loc, std::ios::out );

	if( !file ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create and open file: " + loc );
		return Core::ReturnVar( false );
	}

	if( !contents.empty() )
		file << contents;

	file.close();

	Core::logger.AddLogString( LogLevels::ALL, "Successfully created file: " + loc + " with contents spanning " + std::to_string( contents.size() ) + " bytes" );
	return Core::ReturnVar( true );
}

bool FS::CreateFileIfNotExists( const std::string & loc, const std::string & contents )
{
	Core::logger.AddLogSection( "FS" );
	Core::logger.AddLogSection( "CreateFileIfNotExists" );

	if( loc.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Filename is empty" );
		return Core::ReturnVar( false );
	}

	if( !CreateFileDir( loc ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "FS::CreateFileDir failed" );
		return Core::ReturnVar( false );
	}

	std::fstream file;
	file.open( loc, std::ios::out | std::ios::app );

	if( !file ) {
		Core::logger.AddLogString( LogLevels::ALL, "Unable to create and/or open file: " + loc );
		return Core::ReturnVar( false );
	}

	if( !contents.empty() )
		file << contents;

	file.close();

	Core::logger.AddLogString( LogLevels::ALL, "Successfully created file: " + loc + " with contents spanning " + std::to_string( contents.size() ) + " bytes" );
	return Core::ReturnVar( true );
}

std::vector< std::string > FS::GetFilesInDir( const std::string & loc, const std::regex & regex, const std::vector< std::string > & except )
{
	std::string fdir = loc;

	Env::ReplaceTildeWithHome( fdir );

	if( !fdir.empty() && * fdir.begin() == '.' ) {
		fdir.erase( fdir.begin() );
		if( !fdir.empty() && * fdir.begin() == '/' )
			fdir.erase( fdir.begin() );
	}

	if( fdir.size() > 0 && * ( fdir.end() - 1 ) != '/' )
		fdir += "/";

	DIR * dir;
	struct dirent * ent;

	std::string dir_to_open = fdir.empty() ? "." : fdir;

	std::vector< std::string > res;

	if( ( dir = opendir( dir_to_open.c_str() ) ) != NULL ) {
		while( ( ent = readdir( dir ) ) != NULL ) {
			if( strcmp( ent->d_name, "." ) == 0 || strcmp( ent->d_name, ".." ) == 0 )
				continue;
			if( strncmp( ent->d_name, ".", strlen( "." ) ) == 0 )
				continue;

			std::string loc_name = fdir.empty() ? ent->d_name : fdir + ent->d_name;

			if( ent->d_type == DT_DIR ) {
				auto tempres = GetFilesInDir( loc_name, regex, except );
				res.insert( res.end(), tempres.begin(), tempres.end() );
				continue;
			}
			if( std::regex_match( loc_name, regex ) && std::find( except.begin(), except.end(), loc_name ) == except.end() ) {
				res.push_back( loc_name );
			}
		}
	}

	return res;
}

std::string FS::ReadFile( const std::string & filename )
{
	std::fstream file;

	file.open( filename, std::ios::in );

	if( !file )
		return "";

	std::string res;
	std::string temp;

	while( std::getline( file, temp ) )
		res += temp + "\n";

	file.close();

	return res;
}