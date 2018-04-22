#ifndef STRING_FUNCS_HPP
#define STRING_FUNCS_HPP

#include <vector>
#include <string>
#include <map>

namespace Str
{
	std::vector< std::string > DoublePtrToVector( const int argc, const char ** & argv );

	std::vector< std::string > Delimit( const std::string & str, const char ch = ' ' );

	std::string ToLower( const std::string & str );

	std::string ToUpper( const std::string & str );

	int FetchArgs( const std::vector< std::string > & args, std::map< std::string, std::string > & argmap, int from );

	bool Replace( std::string & str, const std::string & from, const std::string & to );
}

#endif // STRING_FUNCS_HPP