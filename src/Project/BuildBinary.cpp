#include <vector>
#include <string>
#include <array>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/ExecuteCommand.hpp"

#include "../../include/Project/Config.hpp"
#include "../../include/Project/BuildCommon.hpp"

#include "../../include/Project/BuildBinary.hpp"

int Project::BuildBinary( const Config::ProjectData & data, const int data_i, const bool disp_cmds_only )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildBinary" );

	Common::CompileVars cvars;

	Common::GetVars( data, data_i, cvars );

	int total_sources = cvars.files.size() + ( int )!cvars.main_src.empty();

	if( !disp_cmds_only ) Core::logger.AddLogString( LogLevels::ALL, "Compiling " + std::to_string( total_sources ) + " sources with main_src as: " + cvars.main_src );

	int ctr = 1;
	bool is_any_single_file_compiled = false;
	std::string build_files_str;

	int ret = Common::CompileSources( data, data_i, cvars, build_files_str, Common::BuildType::BIN, disp_cmds_only, is_any_single_file_compiled, ctr );

	// ret is less than zero when disp_cmds_only is true
	if( ret < 0 ) return Core::ReturnVar( 0 );
	if( ret != 0 ) return Core::ReturnVar( ret );

	if( !cvars.main_src.empty() ) {
		if( !FS::LocExists( cvars.main_src ) ) {
			Display( "\n{fc}Main source{0}: {r}" + cvars.main_src + " {fc}is defined but the file does not exist{0}\n" );
			Core::logger.AddLogString( LogLevels::ALL, "Main source: " + cvars.main_src + " is defined but the file does not exist" );
			return Core::ReturnVar( 1 );
		}

		int percent = ( ctr * 100 / total_sources );

		bool are_all_latest = true;

		if( FS::LocExists( "bin/" + data.builds[ data_i ].name ) ) {
			for( auto src : cvars.files ) {
				if( !FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, src ) ) are_all_latest = false;
			}
		}
		else {
			are_all_latest = false;
		}

		// Check if there already exists a build whose modification time is newer than main source and / or
		if( !is_any_single_file_compiled && are_all_latest && FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, cvars.main_src ) &&
		    FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, "ccp4m.yaml" ) ) {
			Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{bg}Project is already up to date{0}\n" );
			return Core::ReturnVar( 0 );
		}

		Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + cvars.caps_lang + " binary{0}:   {sc}buildfiles/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string compile_str = cvars.compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ cvars.inc_flags + " -g -o buildfiles/" + data.builds[ data_i ].name + " " + cvars.main_src + " " + build_files_str + " " + cvars.lib_flags;

		if( Core::arch == Core::BSD ) compile_str += " -I/usr/local/include -L/usr/local/lib";

		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() ) Display( "{fc}Error{0}:\n{r}" + err );
			return Core::ReturnVar( ret_val );
		}

		Display( "\n{fc}Moving {sc}buildfiles/" + data.builds[ data_i ].name + "{fc} to {sc}bin/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string cmd = "mv buildfiles/" + data.builds[ data_i ].name + " bin/";
		ret_val = Exec::ExecuteCommand( cmd, & err );
		if( ret_val != 0 ) {
			if( !err.empty() ) Display( "{fc}Error{0}: {r}" + err );
			return Core::ReturnVar( ret_val );
		}
	}

	return Core::ReturnVar( 0 );
}
