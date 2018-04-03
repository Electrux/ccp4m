#include <vector>
#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/DisplayFuncs.hpp"
#include "../../include/ExecuteCommand.hpp"

#include "../../include/Project/Config.hpp"
#include "../../include/Project/BuildCommon.hpp"

#include "../../include/Project/BuildLibrary.hpp"

int Project::BuildLibrary( const ProjectData & data, const int data_i )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildLibrary" );

	std::string main_src;
	std::vector< std::string > files;

	std::string caps_lang;
	std::string compiler;
	std::string inc_flags, lib_flags;

	Common::GetVars( data, data_i, main_src, compiler, caps_lang, inc_flags, lib_flags, files );

	Core::logger.AddLogString( LogLevels::ALL, "Compiling " + std::to_string( files.size() ) + " sources with main_src as: " + main_src );

	if( !Common::CreateSourceDirs( files ) )
		return Core::ReturnVar( 1 );

	int total_sources = files.size() + ( int )!main_src.empty();

	Core::logger.AddLogString( LogLevels::ALL, "Building target: " + data.builds[ data_i ].name + " with " + std::to_string( total_sources ) + " sources" );

	Display( "{fc}Building target {sc}" + data.builds[ data_i ].name + " {fc}with {sc}" + std::to_string( total_sources ) + " {fc}sources {0}...\n\n" );

	std::string build_files_str;

	int ctr = 1;
	bool is_any_single_file_compiled = false;

	for( auto src : files ) {
		int percent = ( ctr * 100 / total_sources );

		build_files_str += "buildfiles/" + src + ".o ";

		// Remove files which are up to date
		if( FS::IsFileLatest( "buildfiles/" + src + ".o", src ) ) {
			Display( "{tc}[" + std::to_string( percent ) + "%]\t{g}Up to date " + caps_lang + " object: {sc}buildfiles/" + src + ".o {0}\n" );
			++ctr;
			continue;
		}

		Display( "{tc}[" + std::to_string( percent ) + "%]\t{fc}Compiling " + caps_lang + " object:  {sc}buildfiles/" + src + ".o {0}...\n" );

		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";

		is_any_single_file_compiled = true;

		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}
		++ctr;
	}

	std::string ext = data.builds[ data_i ].build_type == "static" ? ".a" : ".so";
	std::string lib_type = data.builds[ data_i ].build_type;
	std::string compile_str;

	if( lib_type == "static" ) {
		compile_str = "ar rcs buildfiles/lib" + data.builds[ data_i ].name + ext + " " + main_src + " " + build_files_str;
	}
	else {
		compile_str = compiler + " -shared " + data.compile_flags + " -std=" + data.lang + data.std + " "
				+ inc_flags + " " + lib_flags + " -o buildfiles/lib" + data.builds[ data_i ].name + ".so " + main_src + " " + build_files_str;
	}

	if( !main_src.empty() ) {
		int percent = ( ctr * 100 / total_sources );

		// Check if there already exists a build whose modification time is newer than main source and / or
		if( !is_any_single_file_compiled && FS::IsFileLatest( "lib/" + data.builds[ data_i ].name + ext, main_src ) &&
			FS::IsFileLatest( "lib/" + data.builds[ data_i ].name + ext, "ccp4m.yaml" ) ) {

			Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{bg}Project is already up to date{0}\n" );
			return Core::ReturnVar( 0 );
		}
		Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + caps_lang + " " + lib_type + " library:   {sc}buildfiles/lib" +
			data.builds[ data_i ].name + ext + " {0}...\n" );

		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}

		Display( "\n{fc}Moving {sc}buildfiles/lib" + data.builds[ data_i ].name + ext + "{fc} to {sc}lib/lib" + data.builds[ data_i ].name + ext + " {0}...\n" );
		std::string cmd = "mv buildfiles/lib" + data.builds[ data_i ].name + ext + " lib/";
		ret_val = Exec::ExecuteCommand( cmd, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}
	}

	return Core::ReturnVar( 0 );
}