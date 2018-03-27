#ifndef EXECUTE_COMMAND_HPP
#define EXECUTE_COMMAND_HPP

#include <string>

namespace Exec
{
	int ExecuteCommand( const std::string & cmd, std::string * err = nullptr, std::string * out = nullptr, bool disp_out = true, int thread = 0 );
}

#endif // EXECUTE_COMMAND_HPP