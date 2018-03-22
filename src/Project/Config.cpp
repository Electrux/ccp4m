#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Environment.hpp"
#include "../../include/Vars.hpp"

#include "../../include/Project/Config.hpp"

// Helper function to avoid crash for YAML-CPP
template< typename T > std::string GetString( T & t, const std::string & v )
{
	if( t[ v ] )
		return t[ v ].template as< std::string >();

	return "";
}
template< typename T > std::string GetString( T & t, const std::string & v, const std::string & v2 )
{
	if( t[ v ] && t[ v ][ v2 ] )
		return t[ v ][ v2 ].template as< std::string >();

	return "";
}
template< typename T > std::vector< std::string > GetStringVector( T & t, const std::string & v )
{
	std::vector< std::string > res;
	if( t[ v ] ) {
		for( auto val : t[ v ] ) {
			res.push_back( val.template as< std::string >() );
		}
	}

	return res;
}
template< typename T > std::vector< std::string > GetStringVector( T & t, const std::string & v, const std::string & v2 )
{
	std::vector< std::string > res;
	if( t[ v ] && t[ v ][ v2 ] ) {
		for( auto val : t[ v ][ v2 ] ) {
			res.push_back( val.template as< std::string >() );
		}
	}

	return res;
}

void AddLibrary( const Library & lib );
void AddBuild( const Build & build );

ProjectData & GetData();

bool ProjectConfig::GetDefaultAuthor()
{
	YAML::Node conf = YAML::LoadFile( Env::CCP4M_CONFIG_FILE );

	auto v = Vars::GetSingleton();

	pdata.author.name = v->Replace( GetString( conf, "name" ) );
	pdata.author.email = v->Replace( GetString( conf, "email" ) );

	if( pdata.author.name.empty() || pdata.author.email.empty() ) {
		Display( "{r}Unable to fetch name and email from system config.{0}" );
		return false;
	}

	return true;
}

bool ProjectConfig::GenerateDefaultConfig()
{
	if( !this->GetDefaultAuthor() )
		return false;

	pdata.name = "DefaultProject";
	pdata.version = "0.1";
	pdata.lang = "c++";
	pdata.std = "14";
	pdata.type = "bin";
	pdata.compile_flags = "-O2";

	// Will be set when the build command is called
	pdata.build_date = "None";

	Build build;
	build.name = "DefaultProject";
	build.main_src = "src/main.cpp";

	return true;
}

bool ProjectConfig::LoadFile( const std::string & file )
{
	if( !FS::LocExists( file ) )
		return false;

	YAML::Node conf = YAML::LoadFile( file );

	auto v = Vars::GetSingleton();

	pdata.name = v->Replace( GetString( conf, "name" ) );
	pdata.version = v->Replace( GetString( conf, "version" ) );
	pdata.lang = v->Replace( GetString( conf, "lang" ) );
	pdata.std = v->Replace( GetString( conf, "std" ) );
	pdata.type = v->Replace( GetString( conf, "type" ) );
	pdata.compile_flags = v->Replace( GetString( conf, "compile_flags" ) );
	pdata.build_date = v->Replace( GetString( conf, "build_date" ) );

	pdata.author.name = v->Replace( GetString( conf, "author", "name" ) );
	pdata.author.email = v->Replace( GetString( conf, "author", "email" ) );

	if( ( pdata.author.name.empty() || pdata.author.email.empty() ) && !this->GetDefaultAuthor() ) {
		return false;
	}

	for( auto libdata : conf[ "libs" ] ) {
		Library lib;

		lib.name = v->Replace( GetString( libdata, "name" ) );
		lib.version = v->Replace( GetString( libdata, "version" ) );
		lib.inc_flags = v->Replace( GetString( libdata, "inc_flags" ) );
		lib.lib_flags = v->Replace( GetString( libdata, "lib_flags" ) );

		pdata.libs.push_back( lib );
	}

	for( auto builddata : conf[ "builds" ] ) {
		Build build;

		build.name = v->Replace( GetString( builddata, "name" ) );
		build.main_src = v->Replace( GetString( builddata, "main_src" ) );

		build.srcs = v->Replace( GetStringVector( builddata, "other_src" ) );

		pdata.builds.push_back( build );
	}
}

bool ProjectConfig::SaveFile( const std::string & file )
{
	if( !FS::CreateFile( file ) ) {
		return false;
	}

	YAML::Emitter o;


}

void ProjectConfig::DisplayAll()
{
	Display( "{by}------------------------------------------------\n\n" );
	Display( "{bm}Name: {bg}" + pdata.name + "\n" );
	Display( "{bm}Version: {bg}" + pdata.version + "\n" );
	Display( "{bm}Lang: {bg}" + pdata.lang + "\n" );
	Display( "{bm}Std: {bg}" + pdata.std + "\n" );
	Display( "{bm}Type: {bg}" + pdata.type + "\n" );
	Display( "{bm}Compile_Flags: {bg}" + pdata.compile_flags + "\n\n" );

	Display( "{by}Libs:\n" );

	for( auto lib : pdata.libs ) {
		Display( "\t{bm}Name: {bg}" + lib.name + "\n" );
		Display( "\t{bm}Version: {bg}" + lib.version + "\n" );
		Display( "\t{bm}Inc_Flags: {bg}" + lib.inc_flags + "\n" );
		Display( "\t{bm}Lib_Flags: {bg}" + lib.lib_flags + "\n\n" );
	}

	Display( "\n" );

	Display( "{by}Builds:\n" );

	for( auto build : pdata.builds ) {
		Display( "\t{bm}Name: {bg}" + build.name + "\n" );
		Display( "\t{bm}Main source: {bg}" + build.main_src + "\n" );
		Display( "\t{bm}Other sources:\n" );
		for( auto s : build.srcs ) {
			Display( "\t\t{bg}" + s + "\n" );
		}
	}

	Display( "\n{by}--------------------------------------------------{0}\n\n");
}