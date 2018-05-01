#ifndef EXECUTE_COMMAND_HPP
#define EXECUTE_COMMAND_HPP

#include <vector>
#include <string>
#include <atomic>

namespace Exec
{
	namespace Internal
	{
		struct Result
		{
			int res;
			std::string err;
			std::string src;
		};
		extern std::atomic< int > threadctr;
	}

	int ExecuteCommand( const std::string & cmd, std::string * err = nullptr );

	Internal::Result MultithreadedExecute( const std::string & cmd, const std::string & src );
}

#endif // EXECUTE_COMMAND_HPP