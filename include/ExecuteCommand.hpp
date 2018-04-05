#ifndef EXECUTE_COMMAND_HPP
#define EXECUTE_COMMAND_HPP

#include <vector>
#include <string>
#include <mutex>

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

		extern std::vector< Result > threadinfo;
	}

	int ExecuteCommand( const std::string & cmd, std::string * err = nullptr );

	void MultithreadedExecute( const std::string & cmd, int thread, const std::string & src );
}

#endif // EXECUTE_COMMAND_HPP