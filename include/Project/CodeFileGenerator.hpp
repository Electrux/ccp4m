#ifndef PROJECT_CODE_FILE_GENERATOR_HPP
#define PROJECT_CODE_FILE_GENERATOR_HPP

#include "Config.hpp"

namespace Project
{
	int GenerateIncludeFile( ProjectConfig & pconf, const std::string & file );

	int GenerateSourceFile( ProjectConfig & pconf, const std::string & file, const int which_build );

	int GenerateTestFile( ProjectConfig & pconf, const std::string & file );
}

#endif // PROJECT_CODE_FILE_GENERATOR_HPP