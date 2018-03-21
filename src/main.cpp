#include <iostream>

#include "../include/DateTime.hpp"
#include "../include/GlobalData.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/StringFuncs.hpp"

int main( int argc, char ** argv )
{
	std::string logfile = DT::GetCurrDateTime() + ".log";
	FS::CreateFile( logfile );
	Global::InitLogger( logfile );

	auto args = STR::DoublePtrToVector( argc, ( const char ** & )argv );


	Global::EndLogger();
	return 0;
}
