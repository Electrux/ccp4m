#ifndef CODE_FILE_GENERATOR_HPP
#define CODE_FILE_GENERATOR_HPP

#include "Config.hpp"

namespace Project
{
	int GenerateIncludeFile( ProjectConfig & pconf, const std::string & file );

	int GenerateSourceFile( ProjectConfig & pconf, const std::string & file, const int which_build );

	int GenerateTestFile( ProjectConfig & pconf, const std::string & file );
}

#endif // CODE_FILE_GENERATOR_HPP