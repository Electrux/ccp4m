#include <vector>
#include <string>

#include "../include/StringFuncs.hpp"

std::vector< std::string > STR::DoublePtrToVector( const int argc, const char ** & argv )
{
	std::vector< std::string > vec;

	for( int i = 0; i < argc; ++i )
		vec.push_back( argv[ i ] );

	return vec;
}