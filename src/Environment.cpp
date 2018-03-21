#include <string>
#include <cstdlib>

#include "../include/Environment.hpp"

std::string Env::GetEnvVar( const std::string & key )
{
	return std::getenv( key.c_str() );
}