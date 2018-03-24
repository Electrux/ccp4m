#ifndef FS_FUNCS_HPP
#define FS_FUNCS_HPP

#include <string>
#include <regex>

namespace FS
{
	bool LocExists( const std::string & loc );

	bool CreateDir( const std::string & dir );

	bool CreateFile( const std::string & loc, const std::string & contents = "" );

	bool CreateFileIfNotExists( const std::string & loc, const std::string & contents = "" );

	std::vector< std::string > GetFilesInDir( const std::string & dir = ".", const std::regex & regex = std::regex( "(.*)" ),
		const std::vector< std::string > & except = std::vector< std::string >() );
}

#endif // FS_FUNCS_HPP