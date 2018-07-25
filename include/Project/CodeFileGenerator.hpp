#ifndef PROJECT_CODE_FILE_GENERATOR_HPP
#define PROJECT_CODE_FILE_GENERATOR_HPP

#include "Config.hpp"

namespace Project
{
	/*
	 * desc: generates include file in the include directory ( file parameter ) and adds license information in it with include guards
	 *       according to the project configuration
	 *
	 * param: reference to the project configuration; const is not used because GetData() does not have const in itself
	 * param: const reference to the filename which is required to be created
	 *
	 * return: 0 on success, non-zero on failure
	 */
	int GenerateIncludeFile( ProjectConfig & pconf, const std::string & file );

	/*
	 * desc: generates source file in the src directory ( file parameter ) and adds license information in it
	 *       according to the project configuration
	 *
	 * param: reference to the project configuration; const is not used because GetData() does not have const in itself
	          and configuration is updated to include this source in builds
	 * param: const reference to the filename which is required to be created
	 * param: build index in which the source file is added; if it is -1, source is added to all builds
	 *
	 * return: 0 on success, non-zero on failure
	 */
	int GenerateSourceFile( ProjectConfig & pconf, const std::string & file, const int which_build );

	/*
	 * desc: generates test file in the tests directory ( file parameter ) and adds license information in it
	 *       according to the project configuration
	 *
	 * param: reference to the project configuration; const is not used because GetData() does not have const in itself
	          and configuration is updated to include this source in builds
	 * param: const reference to the filename which is required to be created
	 *
	 * return: 0 on success, non-zero on failure
	 */
	int GenerateTestFile( ProjectConfig & pconf, const std::string & file, const std::string & ext );
}

#endif // PROJECT_CODE_FILE_GENERATOR_HPP
