#include <iostream>

#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/Core.hpp"
#include "../../include/Environment.hpp"
#include "../../include/Vars.hpp"
#include "../../include/Project/Licenses.hpp"

int Exit( int err_code )
{
	Core::EndLogger();
	return err_code;
}

int main( int argc, char ** argv )
{
	std::string logfile = Env::CCP4M_LOG_DIR + "/" + Core::GetCurrDateTime() + ".log";
	FS::CreateFileIfNotExists( logfile );
	if( !Core::InitLogger( logfile ) ) {
		Display( "{br}Failed to initialize logging engine!{0}" );
		return 1;
	}

	if( !Core::InitCore() )
		return Exit( 1 );

	Vars::Initialize();

	int err_code = 0;

	std::string lic = License::FetchLicense( License::gnulessergpl2_1 );

	return Exit( err_code );
}