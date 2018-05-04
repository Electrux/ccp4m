#ifndef PROJECT_BUILD_LIBRARY_HPP
#define PROJECT_BUILD_LIBRARY_HPP

#include "Config.hpp"

namespace Project
{
	int BuildLibrary( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only = false );
}

#endif // PROJECT_BUILD_LIBRARY_HPP