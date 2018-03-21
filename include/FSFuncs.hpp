#ifndef FS_FUNCS_HPP
#define FS_FUNCS_HPP

#include <string>

namespace FS
{
	bool LocExists( const std::string & loc );

	bool CreateDir( const std::string & dir );

	bool CreateFile( const std::string & loc, const std::string & contents = "" );
}

#endif // FS_FUNCS_HPP