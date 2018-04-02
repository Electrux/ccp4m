#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

namespace NW
{
	int DownloadFile( const std::string & url, const std::string & out_loc = "" );
}

#endif // NETWORK_HPP