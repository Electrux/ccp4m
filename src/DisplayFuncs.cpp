#include <iostream>
#include <string>

#include "../include/Vars.hpp"

#include "../include/DisplayFuncs.hpp"

void Display( std::string data )
{
	Vars::ReplaceVars( data );
	std::cout << data;
	std::cout.flush();
}