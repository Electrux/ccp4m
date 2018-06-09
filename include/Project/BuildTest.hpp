/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.

	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PROJECT_BUILD_TEST_HPP
#define PROJECT_BUILD_TEST_HPP

#include "Config.hpp"

namespace Project
{
	/*
	 * desc: generates test binary described by the data_i'th build in project configuration
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: if this boolean is true, the build will not be compiled, instead, the commands to compile it
	 *        will be displayed on the console
	 *
	 * return: 0 on successful compilation, non-zero otherwise
	 */
	int BuildTest( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only = false );
}

#endif // PROJECT_BUILD_TEST_HPP
