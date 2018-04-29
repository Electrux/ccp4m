#ifndef BUILD_COMMON_HPP
#define BUILD_COMMON_HPP

#include <vector>
#include <string>

#include "Config.hpp"

namespace Common
{
	struct CompileVars
	{
		std::string compiler;
		std::string caps_lang;
		std::string inc_flags;
		std::string lib_flags;
		std::vector< std::string > files;
		std::string main_src;
	};

	enum class BuildType
	{
		BIN,
		LIB,
		TEST,
	};

	void GetFlags( const ProjectData & data, std::string & inc_flags, std::string & lib_flags );

	bool CreateSourceDirs( const std::vector< std::string > & srcs );

	void GetVars( const ProjectData & data, int data_i, CompileVars & cvars );

	void DisplayBuildCommands( const ProjectData & data, const int data_i, const CompileVars & cvars, const BuildType & build_type );

	int CompileSources( const ProjectData & data, const int data_i, const CompileVars & cvars, std::string & build_files_str, const BuildType build_type, const bool disp_cmds_only,
		bool & is_any_single_file_compiled, int & ctr );
}

#endif // BUILD_COMMON_HPP