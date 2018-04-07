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

int Project::BuildBinary( const ProjectData & data, const int data_i )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "BuildBinary" );

	std::string main_src;
	std::vector< std::string > files;

	std::string caps_lang;
	std::string compiler;
	std::string inc_flags, lib_flags;

	Common::GetVars( data, data_i, main_src, compiler, caps_lang, inc_flags, lib_flags, files );

	int total_sources = files.size() + ( int )!main_src.empty();

	Core::logger.AddLogString( LogLevels::ALL, "Compiling " + std::to_string( total_sources ) + " sources with main_src as: " + main_src );

	if( !Common::CreateSourceDirs( files ) )
		return Core::ReturnVar( 1 );

	Core::logger.AddLogString( LogLevels::ALL, "Building target: " + data.builds[ data_i ].name );

	Display( "\n{fc}Building target {sc}" + data.builds[ data_i ].name + " {fc}with {sc}" + std::to_string( total_sources ) + " {fc}sources {0}...\n\n" );

	std::string build_files_str;

	int ctr = 1;
	bool is_any_single_file_compiled = false;

	int cores = std::thread::hardware_concurrency() / 2;

	std::vector< std::thread > allthreads;

	for( int i = 0; i < cores; ++i ) {
		Exec::Internal::threadinfo.push_back( { std::numeric_limits< int >::min(), "", "" } );
	}

	Display( "{fc}Using cores: {sc}" + std::to_string( cores ) + "{0}\n\n" );

	for( auto src : files ) {
		int percent = ( ctr * 100 / total_sources );

		build_files_str += "buildfiles/" + src + ".o ";

		// Remove files which are up to date
		if( FS::IsFileLatest( "buildfiles/" + src + ".o", src ) ) {
			Display( "{tc}[" + std::to_string( percent ) + "%]\t{g}Up to date " + caps_lang + " object: {sc}buildfiles/" + src + ".o {0}\n" );
			++ctr;
			continue;
		}

		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";

		is_any_single_file_compiled = true;

		bool thread_found = false;

		while( !thread_found ) {

			for( auto & i : Exec::Internal::threadinfo ) {
				if( i.res != std::numeric_limits< int >::min() && i.res != -1 ) {
					if( i.res != 0 ) {
						Display( "{fc}Error in source: {sc}" + i.src + " {0}:\n" + i.err );
						for( auto & thread : allthreads )
							thread.join();
						return Core::ReturnVar( i.res );
					}
					i.res = std::numeric_limits< int >::min();
					i.err.clear();
				}
			}

			for( int i = 0; i < cores; ++i ) {
				if( Exec::Internal::threadinfo[ i ].res == std::numeric_limits< int >::min() ) {
					Display( "{tc}[" + std::to_string( percent ) + "%]\t{fc}Compiling " + caps_lang + " object:  {sc}buildfiles/" + src + ".o {0}...\n" );
					allthreads.emplace_back( Exec::MultithreadedExecute, compile_str, i, src );
					thread_found = true;
					Exec::Internal::threadinfo[ i ].res = -1;
					break;
				}
			}
		}

		++ctr;
	}

	bool all_done = false;

	while( !all_done ) {
		all_done = true;
		for( auto & i : Exec::Internal::threadinfo ) {
			if( i.res == -1 ) {
				all_done = false;
				continue;
			}
			if( i.res != std::numeric_limits< int >::min() ) {
				if( i.res != 0 ) {
					Display( "{fc}Error in source: {sc}" + i.src + " {0}:\n" + i.err );
					for( auto & thread : allthreads )
						thread.join();
					return Core::ReturnVar( i.res );
				}
				i.res = std::numeric_limits< int >::min();
				i.err.clear();
				all_done = false;
			}
		}
	}

	for( auto & thread : allthreads )
		thread.join();

	Exec::Internal::threadinfo.clear();

	if( !main_src.empty() ) {
		int percent = ( ctr * 100 / total_sources );

		bool are_all_latest = true;

		if( FS::LocExists( "bin/" + data.builds[ data_i ].name ) ) {
			for( auto src : files ) {
				if( !FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, src ) )
					are_all_latest = false;
			}
		}
		else {
			are_all_latest = false;
		}

		// Check if there already exists a build whose modification time is newer than main source and / or
		if( !is_any_single_file_compiled && are_all_latest && FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, main_src ) &&
			FS::IsFileLatest( "bin/" + data.builds[ data_i ].name, "ccp4m.yaml" ) ) {

			Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{bg}Project is already up to date{0}\n" );
			return Core::ReturnVar( 0 );
		}

		Display( "\n{tc}[" + std::to_string( percent ) + "%]\t{fc}Building " + caps_lang + " binary:   {sc}buildfiles/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string compile_str = compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ inc_flags + " " + lib_flags + " -g -o buildfiles/" + data.builds[ data_i ].name + " " + main_src + " " + build_files_str;
		std::string err;
		int ret_val = Exec::ExecuteCommand( compile_str, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}

		Display( "\n{fc}Moving {sc}buildfiles/" + data.builds[ data_i ].name + "{fc} to {sc}bin/" + data.builds[ data_i ].name + " {0}...\n" );
		std::string cmd = "mv buildfiles/" + data.builds[ data_i ].name + " bin/";
		ret_val = Exec::ExecuteCommand( cmd, & err );
		if( ret_val != 0 ) {
			if( !err.empty() )
				Display( "{fc}Error: {r}" + err );
			return Core::ReturnVar( ret_val );
		}
	}

	return Core::ReturnVar( 0 );
}