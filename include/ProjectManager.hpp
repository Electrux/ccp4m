#ifndef PROJECT_MANAGER_HPP
#define PROJECT_MANAGER_HPP

#include <vector>
#include <string>

namespace Project
{
	int Handle( const std::vector< std::string > & args );

	int Build( const std::vector< std::string > & args );

	int New( const std::vector< std::string > & args );

	int Add( const std::vector< std::string > & args );

	int Set( const std::vector< std::string > & args );

	int Run( const std::vector< std::string > & args );

	int Clean();
}

#endif // PROJECT_MANAGER_HPP