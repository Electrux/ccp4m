#include <vector>
#include <string>

#include "../include/StringFuncs.hpp"

std::vector< std::string > Str::DoublePtrToVector( const int argc, const char ** & argv )
{
	std::vector< std::string > vec;

	for( int i = 0; i < argc; ++i )
		vec.push_back( argv[ i ] );

	return vec;
}

std::vector< std::string > Str::Delimit( const std::string & str, const char ch )
{
	std::string temp;
	std::vector< std::string > vec;

	for( auto c : str ) {
		if( c == ch ) {
			if( temp.empty() )
				continue;
			vec.push_back( temp );
			temp.clear();
			continue;
		}

		temp += c;
	}

	return vec;
}

std::string Str::ToLower( const std::string & str )
{
	std::string res;
	for( auto c : str ) {
		res += std::tolower( c );
	}

	return res;
}

std::string Str::ToUpper( const std::string & str )
{
	std::string res;
	for( auto c : str ) {
		res += std::toupper( c );
	}

	return res;
}