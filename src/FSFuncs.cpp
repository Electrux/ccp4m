#include <string>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>

#include "../include/Environment.hpp"

#include "../include/FSFuncs.hpp"

bool FS::LocExists( const std::string & loc )
{
	if( loc.empty() )
		return true;

	struct stat info;

	return stat( loc.c_str(), & info ) == 0 || lstat( loc.c_str(), & info ) == 0;
}

bool FS::CreateDir( const std::string & dir )
{
	if( LocExists( dir ) )
		return true;

	int ctr = 0;
	std::string fdir;

	for( auto ch : dir ) {
		if( ch == '~' ) {
			fdir += Env::GetEnvVar( "HOME" );
		}
		else {
			fdir += ch;
		}
		if( ch == '/' && !LocExists( fdir ) ) {
			if( mkdir( fdir.c_str(), 0755 ) != 0 )
				return false;
		}
	}

	return !LocExists( fdir ) ? mkdir( fdir.c_str(), 0755 ) == 0 : true;
}

bool FS::CreateFile( const std::string & loc, const std::string & contents = "" )
{
	auto last_slash = loc.rfind( '/' );
	if( last_slash != std::string::npos ) {
		std::string dir = loc.substr( 0, last_slash );
		if( !LocExists( dir ) && !CreateDir( dir ) )
			return false;
	}

	std::fstream file;
	file.open( loc, std::ios::out );

	if( !file )
		return false;

	if( !contents.empty() )
		file << contents;

	file.close();

	return true;
}