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
	for( auto src : files ) {
		int percent = ( ctr * 100 / total_sources );
		Display( "{tc}[" + std::to_string( percent ) + "%]\t{fc}Compiling " + caps_lang + " object: {sc}buildfiles/" + src + ".o {0}...\n" );

		build_files_str += "buildfiles/" + src + ".o ";
		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";
		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}
		++ctr;
	}

	if( !main_src.empty() ) {
		int percent = ( ctr * 100 / total_sources );
		if( data.builds[ data_i ].build_type == "static" ) {
			Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + caps_lang + " static library: {sc}buildfiles/lib" + data.builds[ data_i ].name + ".a {0}...\n" );
			std::string compile_str = "ar rcs buildfiles/lib" + data.builds[ data_i ].name + ".a " + main_src + " " + build_files_str;
			std::string err;
			int ret_val = Exec::ExecuteCommand( compile_str, & err );
			if( ret_val != 0 ) {
				if( !err.empty() )
					Display( "{fc}Error: {r}" + err );
				return Core::ReturnVar( ret_val );
			}

			Display( "\n{fc}Moving {sc}buildfiles/lib" + data.builds[ data_i ].name + ".a{fc} to {sc}lib/lib" + data.builds[ data_i ].name + ".a {0}...\n" );
			std::string cmd = "mv buildfiles/lib" + data.builds[ data_i ].name + ".a lib/";
			ret_val = Exec::ExecuteCommand( cmd, & err );
			if( ret_val != 0 ) {
				if( !err.empty() )
					Display( "{fc}Error: {r}" + err );
				return Core::ReturnVar( ret_val );
			}
		}
		else {
			Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + caps_lang + " library: {sc}buildfiles/lib" + data.builds[ data_i ].name + ".so {0}...\n" );
			std::string compile_str = compiler + " -shared " + data.compile_flags + " -std=" + data.lang + data.std + " "
				+ inc_flags + " " + lib_flags + " -o buildfiles/lib" + data.builds[ data_i ].name + ".so " + main_src + " " + build_files_str;
			std::string err;
			int ret_val = Exec::ExecuteCommand( compile_str, & err );
			if( ret_val != 0 ) {
				if( !err.empty() )
					Display( "{fc}Error: {r}" + err );
				return Core::ReturnVar( ret_val );
			}

			Display( "\n{fc}Moving {sc}buildfiles/lib" + data.builds[ data_i ].name + ".so{fc} to {sc}lib/lib" + data.builds[ data_i ].name + ".so {0}...\n" );
			std::string cmd = "mv buildfiles/lib" + data.builds[ data_i ].name + ".so lib/";
			ret_val = Exec::ExecuteCommand( cmd, & err );
			if( ret_val != 0 ) {
				if( !err.empty() )
					Display( "{fc}Error: {r}" + err );
				return Core::ReturnVar( ret_val );
			}
		}
	}

	return Core::ReturnVar( 0 );
}