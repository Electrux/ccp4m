/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PROJECT_CREATE_NEW_HPP
#define PROJECT_CREATE_NEW_HPP

#include <vector>
#include <string>
#include <map>

namespace Project
{
	std::map< std::string, std::string > FetchArgs( const std::vector< std::string > & args );
}

#endif // PROJECT_CREATE_NEW_HPP