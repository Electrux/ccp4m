#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <vector>
#include <string>

namespace Env
{
	extern const std::string CCP4M_DIR;
	extern const std::string CCP4M_CONFIG_FILE;

	extern const std::string CCP4M_PROJECT_CONFIG_FILE;

	std::string GetEnvVar( const std::string & key );

	bool SetCurrentDir( const std::string & dir );
	std::string GetCurrentDir();

	std::vector< std::string > GetEnvPathVec();
}

#endif // ENVIRONMENT_HPP