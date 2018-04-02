#include <iostream>
#include <string>

#include "../include/Vars.hpp"

#include "../include/DisplayFuncs.hpp"

void MoveBack( int chars )
{
	for( int i = 0; i < chars; ++i )
		std::cout << "\b \b";

	std::cout.flush();
}

void Display( std::string data )
{
	if( data.empty() )
		return;

	int subtract = 0;

	Vars::ReplaceVars( data );

	for( auto it = data.begin(); it != data.end(); ++it )
		LAST_LINE_SIZE = ( * it == '\n' ) ? 0 : LAST_LINE_SIZE + 1;

	std::cout << data;
	std::cout.flush();
}

void DisplayOneLiner( std::string data, bool store_log )
{
	MoveBack( LAST_ONE_LINER_SIZE );

	if( data.empty() )
		return;

	LAST_ONE_LINER_SIZE = Vars::ReplaceVars( data, false );

	std::cout << data;
	std::cout.flush();
}