/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PROJECT_ADD_HPP
#define PROJECT_ADD_HPP

#include <vector>
#include <string>

#include "Config.hpp"

namespace Project
{
	/*
	 * desc: adds new source/include file to a project, also adds them to project configuration
	 *
	 * param: reference to the project configuration structure
	 * param: arguments provided to the program on the command line
	 *
	 * return: 0 on success, another value on failure
	 */
	int AddFiles( ProjectConfig & pconf, const std::vector< std::string > & args );

	/*
	 * desc: adds new libraries or builds to project configuration
	 *
	 * param: reference to the project configuration structure
	 * param: arguments provided to the program on the command line
	 *
	 * return: 0 on success, another value on failure
	 */
	int AddProjectInfo( ProjectConfig & pconf, const std::vector< std::string > & args );
}

#endif // PROJECT_ADD_HPP
