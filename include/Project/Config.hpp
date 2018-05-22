#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP

#include <string>
#include <vector>

namespace Config
{
	struct Library
	{
		std::string name;
		std::string version;
		std::string inc_flags;
		std::string lib_flags;
	};

	struct Build
	{
		std::string name;
		std::string type;
		std::string build_type;
		std::string inc_flags;
		std::string lib_flags;
		std::string pre_exec;
		std::vector< std::string > srcs;
		std::string main_src;
		std::vector< std::string > exclude;
	};

	struct Author
	{
		std::string name;
		std::string email;
	};

	struct Vars
	{
		std::string name;
		std::string val;
	};

	struct ProjectData
	{
		std::string name;
		Author author;

		std::string version;
		std::string lang;
		std::string std;
		std::string compile_flags;

		std::vector< Vars > vars;

		std::string license;

		std::vector< Library > libs;
		std::vector< Build > builds;
	};
}

class ProjectConfig
{
	Config::ProjectData pdata;

public:

	void AddLibrary( const Config::Library & lib );
	void AddBuild( const Config::Build & build );

	std::string GetDefaultMainFile();

	Config::ProjectData & GetData();

	bool GetDefaultAuthor();

	bool GenerateDefaultConfig();

	bool LoadFile( const std::string & file, bool update_license = true, bool alter_vars = false );

	bool SaveFile( const std::string & file );

	void DisplayAll( const std::string & dir = "" );
};

#endif // PROJECT_CONFIG_HPP
