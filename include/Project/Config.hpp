#ifndef PROJECT_CONFIG_HPP
#define PROJECT_CONFIG_HPP

#include <string>
#include <vector>

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
	std::vector< std::string > srcs;
	std::string main_src;
	std::vector< std::string > exclude;
};

struct Author
{
	std::string name;
	std::string email;
};

struct ProjectData
{
	std::string name;
	Author author;

	std::string version;
	std::string lang;
	std::string std;
	std::string compile_flags;

	std::string build_date;

	std::vector< Library > libs;
	std::vector< Build > builds;
};

class ProjectConfig
{
	ProjectData pdata;

public:

	void AddLibrary( const Library & lib );
	void AddBuild( const Build & build );

	ProjectData & GetData();

	bool GetDefaultAuthor();

	bool GenerateDefaultConfig();

	bool LoadFile( const std::string & file );

	bool SaveFile( const std::string & file );

	void DisplayAll();

};

#endif // PROJECT_CONFIG_HPP