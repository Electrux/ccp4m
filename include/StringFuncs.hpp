#ifndef STRING_FUNCS_HPP
#define STRING_FUNCS_HPP

#include <vector>
#include <string>

namespace Str
{
	std::vector< std::string > DoublePtrToVector( const int argc, const char ** & argv );

	std::vector< std::string > Delimit( const std::string & str, const char ch = ' ' );

	std::string ToLower( const std::string & str );

	std::string ToUpper( const std::string & str );
}

#endif // STRING_FUNCS_HPP