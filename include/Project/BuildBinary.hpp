#ifndef PROJECT_BUILD_BINARY_HPP
#define PROJECT_BUILD_BINARY_HPP

#include "Config.hpp"

namespace Project
{
	/*
	 * desc: compiles a build in configuration of type - binary
	 *
	 * param: const reference to the project configuration structure
	 * param: the index of build to compile in the list of builds within project configuration
	 * param: if this boolean is true, the build will not be compiled, instead, the commands to compile it
	 *        will be displayed on the console
	 *
	 * return: 0 on success, another value on failure
	 */
	int BuildBinary( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only = false );
}

#endif // PROJECT_BUILD_BINARY_HPP
