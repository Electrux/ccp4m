#ifndef PROJECT_BUILD_BINARY_HPP
#define PROJECT_BUILD_BINARY_HPP

#include "Config.hpp"

namespace Project
{
	int BuildBinary( const ProjectData & data, const int data_i );

	void GetFlags( const ProjectData & data, std::string & inc_flags, std::string & lib_flags );

	bool CreateSourceDirs( const std::vector< std::string > & srcs );

	void GetVars( const ProjectData & data, int data_i, std::string & main_src, std::string & compiler, std::string & caps_lang,
		std::string & inc_flags, std::string & lib_flags, std::vector< std::string > & files );
}

#endif // PROJECT_BUILD_BINARY_HPP