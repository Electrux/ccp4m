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
	int AddFiles( ProjectConfig & pconf, const std::vector< std::string > & args );

	int AddProjectInfo( ProjectConfig & pconf, const std::vector< std::string > & args );
}

#endif // PROJECT_ADD_HPP