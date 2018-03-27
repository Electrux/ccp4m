#ifndef BUILD_COMMON_HPP
#define BUILD_COMMON_HPP

#include <vector>
#include <string>

#include "Config.hpp"

namespace Common
{
	void GetFlags( const ProjectData & data, std::string & inc_flags, std::string & lib_flags );

	bool CreateSourceDirs( const std::vector< std::string > & srcs );

	void GetVars( const ProjectData & data, const int data_i, std::string & main_src, std::string & compiler, std::string & caps_lang,
		std::string & inc_flags, std::string & lib_flags, std::vector< std::string > & files );
}

#endif // BUILD_COMMON_HPP