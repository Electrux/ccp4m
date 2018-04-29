#ifndef PROJECT_BUILD_BINARY_HPP
#define PROJECT_BUILD_BINARY_HPP

#include "Config.hpp"

namespace Project
{
	int BuildBinary( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only = false );
}

#endif // PROJECT_BUILD_BINARY_HPP