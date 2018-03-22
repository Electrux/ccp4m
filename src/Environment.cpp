#include <string>
#include <cstdlib>

#include "../include/StringFuncs.hpp"

#include "../include/Environment.hpp"

const std::string Env::CCP4M_DIR = Env::GetEnvVar( "HOME" ) + "/.ccp4m";
const std::string Env::CCP4M_CONFIG_FILE = Env::CCP4M_DIR + "/config.yaml";

const std::string Env::CCP4M_PROJECT_CONFIG_FILE = "ccp4m.yaml";

std::string Env::GetEnvVar( const std::string & key )
{
	return std::getenv( key.c_str() );
}

std::vector< std::string > Env::EnvPathVec()
{
	return Str::Delimit( GetEnvVar( "HOME" ), ':' );
}