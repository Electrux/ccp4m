#include <string>
#include <vector>

#include "../../include/Logger/StringFuncs.hpp"

void ReplaceInString( std::string & str, const std::string & from, const std::string & to )
{
	size_t n = 0;

	while( ( n = str.find( from, n ) ) != std::string::npos ) {
		str.replace( n, from.size(), to );

		n += to.size();
	}
}

std::vector< std::string > DelimitString( const std::string & str, const char delim )
{
	std::vector< std::string > res;
	std::string temp;

	for( auto ch : str ) {
		if( ch == delim ) {
			if( !temp.empty() )
				res.push_back( temp );
			temp.clear();
			continue;
		}
		temp += ch;
	}

	if( !temp.empty() )
		res.push_back( temp );

	return res;
}
