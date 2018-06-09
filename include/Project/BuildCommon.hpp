#ifndef PROJECT_BUILD_COMMON_HPP
#define PROJECT_BUILD_COMMON_HPP

#include <vector>
#include <string>

#include "Config.hpp"

namespace Common
{
	/*
	 * desc: this structure contains variables which are required to compile a build;
	 *       it was made to reduce the parameters passed in a function
	 */
	struct CompileVars
	{
		std::string compiler;
		std::string caps_lang;
		std::string inc_flags;
		std::string lib_flags;
		std::vector< std::string > files;
		std::string main_src;
	};

	/*
	 * desc: describes the build type, this helps in deciding options for compilation
	 */
	enum class BuildType
	{
		BIN,
		LIB,
		TEST,
	};

	/*
	 * desc: fetches the include and library flags for data_i'th build in project configuration
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: reference to include flags - this will be modified in the function
	 * param: reference to library flags - this will be modified in the function
	 */
	void GetFlags( const Config::ProjectData & data, int data_i, std::string & inc_flags, std::string & lib_flags );

	/*
	 * desc: creates the directories for sources before actual compilation
	 *
	 * param: the vector of all sources which are to be compiled

	 * return: true if all directories are successfully created or already existing, false otherwise
	 */
	bool CreateSourceDirs( const std::vector< std::string > & srcs );

	/*
	 * desc: fetches the compile variables for data_i'th build in project configuration
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: reference to the compile variables object - this will be modified in the function
	 */
	void GetVars( const Config::ProjectData & data, int data_i, CompileVars & cvars );

	/*
	 * desc: displays the build commands, used when the build command is appended with --cmds_only;
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: const reference to the compile variables object
	 * param: const reference to the build_type ( note that one can just pass the BuildType::<value> directly instead of creating a variable )
	 */
	void DisplayBuildCommands( const Config::ProjectData & data, const int data_i, const CompileVars & cvars, const BuildType & build_type );

	/*
	 * desc: compiles the sources of the data_i'th build EXCEPT the main source of that build;
	         main source is compiled depending on the build type ( library, binary, test ) and hence is built in respective functions
	 *
	 * param: const reference to project configuration
	 * param: index of build in the project configuration
	 * param: const reference to the compile variables object
	 * param: reference to a variable which stores the name of all object files which are created by function;
	 *        used by the specialized functions ( for library, binary, test ) to add all the objects to use during final compilation - modified by the function
	 * param: const reference to the build_type ( note that one can just pass the BuildType::<value> directly instead of creating a variable )
	 * param: if this boolean is true, the build will not be compiled, instead, the commands to compile it
	 *        will be displayed on the console
	 * param: this variable is set to true if there is at least one file which is compiled by this function - modified by the function
	 * param: this variable counts how many sources have been compiled ( to calculate percentage ) - modified by the function
	 *
	 * return: 0 on successful compilation, non-zero otherwise
	 */
	int CompileSources( const Config::ProjectData & data, const int data_i, const CompileVars & cvars, std::string & build_files_str, const BuildType build_type,
		const bool disp_cmds_only, bool & is_any_single_file_compiled, int & ctr );
}

#endif // PROJECT_BUILD_COMMON_HPP
