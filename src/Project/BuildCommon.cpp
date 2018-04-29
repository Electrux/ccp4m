#include <vector>
#include <string>

#include "../../include/Core.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/ExecuteCommand.hpp"
#include "../../include/DisplayFuncs.hpp"

#include "../../include/Project/Config.hpp"

#include "../../include/Project/BuildCommon.hpp"

void Common::GetFlags( const Config::ProjectData & data, std::string & inc_flags, std::string & lib_flags )
{
	inc_flags = "";
	lib_flags = "";
	for( auto lib : data.libs ) {
		if( !lib.inc_flags.empty() )
			inc_flags += lib.inc_flags + " ";
		if( !lib.lib_flags.empty() )
			lib_flags += lib.lib_flags + " ";
	}
}

bool Common::CreateSourceDirs( const std::vector< std::string > & srcs )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "CreateSourceDirs" );

	for( auto src : srcs ) {
		auto last_slash = src.rfind( '/' );
		if( last_slash != std::string::npos ) {
			std::string dir = "buildfiles/" + src.substr( 0, last_slash );
			if( !FS::LocExists( dir ) ) {
				if( !FS::CreateDir( dir, false ) ) {
					Core::logger.AddLogString( LogLevels::ALL, "Unable to access/create directory: " + dir + " for source file: " + src );
					return Core::ReturnVar( false );
				}
				Core::logger.AddLogString( LogLevels::ALL, "Created build directory: " + dir + " for source file: " + src );
			}
		}
	}

	return Core::ReturnVar( true );
}

void Common::GetVars( const Config::ProjectData & data, int data_i, CompileVars & cvars )
{
	cvars.main_src = data.builds[ data_i ].main_src;

	std::vector< std::string > excludefiles = data.builds[ data_i ].exclude;
	if( !cvars.main_src.empty() )
		excludefiles.push_back( cvars.main_src );

	for( auto src : data.builds[ data_i ].srcs ) {
		std::vector< std::string > tempfiles;
		if( excludefiles.empty() )
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ) );
		else
			tempfiles = FS::GetFilesInDir( ".", std::regex( src ), excludefiles );
		cvars.files.insert( cvars.files.end(), tempfiles.begin(), tempfiles.end() );
	}

	cvars.caps_lang = data.lang == "c++" ? "CXX" : "C";

	if( data.lang == "c++" )
		Core::SetVarArch( cvars.compiler, { "g++", "clang++", "clang++" } );
	else
		Core::SetVarArch( cvars.compiler, { "gcc", "clang", "clang" } );

	GetFlags( data, cvars.inc_flags, cvars.lib_flags );
}

void Common::DisplayBuildCommands( const Config::ProjectData & data, const int data_i, const CompileVars & cvars, const BuildType & build_type )
{
	std::string build_files_str;
	Display( "\n{fc}Build commands for target{0}: {sc}" + data.builds[ data_i ].name + "{fc} are{0} ...\n\n" );

	for( auto src : cvars.files ) {
		build_files_str += "buildfiles/" + src + ".o ";
		std::string compile_str = cvars.compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ cvars.inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";
		if( Core::arch == Core::BSD ) {
			compile_str += " -I/usr/local/include";
		}
		Display( "{tc}" + compile_str + "{0}\n" );
	}

	if( !cvars.main_src.empty() ) {
		std::string compile_str;
		if( build_type == BuildType::BIN || build_type == BuildType::TEST ) {
			compile_str = cvars.compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
				+ cvars.inc_flags + " -g -o buildfiles/" + data.builds[ data_i ].name + " " + cvars.main_src + " " + build_files_str + " " + cvars.lib_flags;
		}
		if( build_type == BuildType::LIB ) {
			std::string ext = data.builds[ data_i ].build_type == "static" ? ".a" : ".so";
			std::string lib_type = data.builds[ data_i ].build_type;

			if( lib_type == "static" ) {
				compile_str = "ar rcs buildfiles/lib" + data.builds[ data_i ].name + ext + " " + cvars.main_src + " " + build_files_str;
			}
			else {
				compile_str = cvars.compiler + " -shared " + data.compile_flags + " -std=" + data.lang + data.std + " "
						+ cvars.inc_flags + " -o buildfiles/lib" + data.builds[ data_i ].name + ".so " + cvars.main_src + " " + build_files_str + " " + cvars.lib_flags;
			}
		}
		if( Core::arch == Core::BSD )
			compile_str += " -I/usr/local/include -L/usr/local/lib";

		Display( "\n{tc}" + compile_str + "{0}\n" );
	}
}

int Common::CompileSources( const Config::ProjectData & data, const int data_i, const CompileVars & cvars, std::string & build_files_str, const BuildType build_type,
	const bool disp_cmds_only, bool & is_any_single_file_compiled, int & ctr )
{
	Core::logger.AddLogSection( "Common" );
	Core::logger.AddLogSection( "CompileSources" );

	if( !CreateSourceDirs( cvars.files ) )
		return Core::ReturnVar( 1 );

	if( disp_cmds_only ) {
		DisplayBuildCommands( data, data_i, cvars, build_type );
		return Core::ReturnVar( -1 );
	}

	int total_sources = cvars.files.size() + ( int )!cvars.main_src.empty();

	Core::logger.AddLogString( LogLevels::ALL, "Building target: " + data.builds[ data_i ].name );
	Display( "\n{fc}Building target {sc}" + data.builds[ data_i ].name + " {fc}with {sc}" + std::to_string( total_sources ) + " {fc}sources {0}...\n\n" );

	int cores = std::thread::hardware_concurrency() / 2;
	std::vector< std::thread > allthreads;

	for( int i = 0; i < cores; ++i ) {
		Exec::Internal::threadinfo.push_back( { std::numeric_limits< int >::min(), "", "" } );
	}

	Display( "{fc}Using cores{0}: {sc}" + std::to_string( cores ) + "{0}\n\n" );

	for( auto src : cvars.files ) {
		int percent = ( ctr * 100 / total_sources );

		build_files_str += "buildfiles/" + src + ".o ";

		// Remove files which are up to date
		if( FS::IsFileLatest( "buildfiles/" + src + ".o", src ) ) {
			Display( "{tc}[" + std::to_string( percent ) + "%]\t{g}Up to date " + cvars.caps_lang + " object{0}: {sc}buildfiles/" + src + ".o {0}\n" );
			++ctr;
			continue;
		}

		std::string compile_str = cvars.compiler + " " + data.compile_flags + " -std=" + data.lang + data.std + " "
			+ cvars.inc_flags + " -c " + src + " -o buildfiles/" + src + ".o";

		if( Core::arch == Core::BSD ) {
			compile_str += " -I/usr/local/include";
		}

		is_any_single_file_compiled = true;

		bool thread_found = false;

		while( !thread_found ) {

			for( auto & i : Exec::Internal::threadinfo ) {
				if( i.res != std::numeric_limits< int >::min() && i.res != -1 ) {
					if( i.res != 0 ) {
						Display( "{fc}Error in source{0}: {sc}" + i.src + " {0}:\n" + i.err );
						for( auto & thread : allthreads )
							thread.join();
						return Core::ReturnVar( i.res );
					}
					if( !i.err.empty() ) {
						Display( "{fc}Warning in source{0}: {sc}" + i.src + " {0}:\n" + i.err );
					}
					i.res = std::numeric_limits< int >::min();
					i.err.clear();
				}
			}

			for( int i = 0; i < cores; ++i ) {
				if( Exec::Internal::threadinfo[ i ].res == std::numeric_limits< int >::min() ) {
					Display( "{tc}[" + std::to_string( percent ) + "%]\t{fc}Compiling " + cvars.caps_lang + " object{0}:  {sc}buildfiles/" + src + ".o {0}...\n" );
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
					Display( "{fc}Error in source{0}: {sc}" + i.src + " {0}:\n" + i.err );
					for( auto & thread : allthreads )
						thread.join();
					return Core::ReturnVar( i.res );
				}
				if( !i.err.empty() ) {
					Display( "{fc}Warning in source{0}: {sc}" + i.src + " {0}:\n" + i.err );
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

	return Core::ReturnVar( 0 );
}