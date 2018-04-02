#ifndef VARS_HPP
#define VARS_HPP

#include <string>
#include <map>

class Vars
{
	std::map< std::string, std::string > vars;

	static Vars * v;

	Vars();
	~Vars();

public:

	void AddVar( const std::string & key, const std::string & val );

	std::string GetVar( const std::string & key );

	bool DeleteVar( const std::string & key );

	static Vars * GetSingleton();

	static void DeleteSingleton();

	static void Initialize();

	// To avoid using static replace function which will cause a lot of overhead if the function is called many times.
	int Replace( std::string & str, bool colors = false );
	// Colors are false because primarily used by project config loader
	std::string Replace( std::string && str, bool colors = false );
	std::vector< std::string > Replace( std::vector< std::string > && vec, bool colors = false );

	// Colors true because primarily used by Display functions
	static int ReplaceVars( std::string & str, bool colors = true );

};

#endif // VARS_HPP