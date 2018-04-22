#ifndef LICENSES_HPP
#define LICENSES_HPP

#include <vector>
#include <string>
#include <map>

namespace License
{
	extern const std::vector< std::string > LICENSES;

	extern const std::vector< std::string > LICENSES_FORMAL;

	enum ID
	{
		apache2_0,
		gnugpl3_0,
		mit,
		bsd2,
		bsd3,
		eclipse2_0,
		gnuafferogpl3_0,
		gnulessergpl2_1,
		gnulessergpl3_0,
		mozilla,
		unlicense,
	};

	extern const ID DEFAULT_LICENSE;

	std::string FetchLicense( const ID & id );

	std::string FetchLicense( const std::string & name );

	std::string FetchLicenseForFile( const ID & id );

	std::string FetchLicenseForFile( const std::string & name );

	std::string FetchLicenseFormalName( const std::string & license );

	bool UpdateProjectLicenseFile( const std::string & license, bool really_update = true );

	bool UpdateProjectSourceFiles( const std::string & old_license, const std::string & license, const std::string & lang );
}

#endif // LICENSES_HPP