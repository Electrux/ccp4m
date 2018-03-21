#ifndef STRINGFUNCS_HPP
#define STRINGFUNCS_HPP

#include <string>

void ReplaceInString( std::string & str, const std::string & from, const std::string & to );

std::vector< std::string > DelimitString( const std::string & str, const char delim = ' ' );

#endif // STRINGFUNCS_HPP
