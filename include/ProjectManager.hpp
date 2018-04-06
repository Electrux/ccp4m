#ifndef PROJECT_MANAGER_HPP
#define PROJECT_MANAGER_HPP

#include <vector>
#include <string>

namespace Project
{
	int Handle( const std::vector< std::string > & args );

	int Build( const std::vector< std::string > & args );

	int Run( const std::vector< std::string > & args );

	int Clean();
}

#endif // PROJECT_MANAGER_HPP