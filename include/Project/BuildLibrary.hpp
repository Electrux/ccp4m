#ifndef PROJECT_BUILD_LIBRARY_HPP
#define PROJECT_BUILD_LIBRARY_HPP

#include "Config.hpp"

namespace Project
{
	/*
	 * desc: generates library files of the data_i'th build in project configuration
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: if this boolean is true, the build will not be compiled, instead, the commands to compile it
	 *        will be displayed on the console
	 *
	 * return: 0 on successful compilation, non-zero otherwise
	 */
	int BuildLibrary( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only = false );
}

#endif // PROJECT_BUILD_LIBRARY_HPP
