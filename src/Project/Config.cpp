#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../../include/DisplayFuncs.hpp"
#include "../../include/FSFuncs.hpp"
#include "../../include/Environment.hpp"
#include "../../include/Vars.hpp"
#include "../../include/Core.hpp"

#include "../../include/Project/Licenses.hpp"

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

void ProjectConfig::AddLibrary( const Library & lib )
{
	if( !lib.name.empty() )
		pdata.libs.push_back( lib );
}

void ProjectConfig::AddBuild( const Build & build )
{
	if( !build.name.empty() )
		pdata.builds.push_back( build );
}

std::string ProjectConfig::GetDefaultMainFile()
{
	std::string res;
	if( this->pdata.lang == "c" ) {
		res = "#include <stdio.h>\n#include <stdlib.h>\n\nint main()\n{\n\treturn 0;\n}";
	}
	else {
		res = "#include <iostream>\n\nint main()\n{\n\treturn 0;\n}";
	}

	return res;
}

ProjectData & ProjectConfig::GetData()
{
	return this->pdata;
}

bool ProjectConfig::GetDefaultAuthor()
{
	Core::logger.AddLogSection( "ProjectConfig" );
	Core::logger.AddLogSection( "GetDefaultAuthor" );

	Core::logger.AddLogString( LogLevels::ALL, "Fetching default author information from system configuration" );

	YAML::Node conf = YAML::LoadFile( Env::CCP4M_CONFIG_FILE );

	auto v = Vars::GetSingleton();

	pdata.author.name = v->Replace( GetString( conf, "name" ) );
	pdata.author.email = v->Replace( GetString( conf, "email" ) );

	if( pdata.author.name.empty() || pdata.author.email.empty() ) {
		Display( "{r}Unable to fetch name and email from system config.{0}" );
		Core::logger.AddLogString( LogLevels::ALL, "Default author information fetch failed. Name and/or email does not exist" );
		return Core::ReturnVar( false );
	}
	v->AddVar( "author", pdata.author.name );

	Core::logger.AddLogString( LogLevels::ALL, "Default author information fetched successfully - Name: " + pdata.author.name + " Email: " + pdata.author.email );
	return Core::ReturnVar( true );
}

bool ProjectConfig::GenerateDefaultConfig()
{
	if( !this->GetDefaultAuthor() )
		return false;

	Core::logger.AddLogSection( "ProjectConfig" );
	Core::logger.AddLogSection( "GenerateDefaultConfig" );

	pdata.name = "UntitledProject";
	pdata.version = "0.1";
	pdata.lang = "c++";
	pdata.std = "14";
	pdata.compile_flags = "-O2";

	pdata.license = License::LICENSES[ License::DEFAULT_LICENSE ];

	Build build;
	build.name = "DefaultBuild";
	build.type = "bin";
	build.main_src = "src/main.cpp";
	build.srcs.push_back( "src/(.*).cpp" );

	pdata.builds.push_back( build );

	return Core::ReturnVar( true );
}

bool ProjectConfig::LoadFile( const std::string & file, bool update_license )
{
	if( !FS::LocExists( file ) )
		return false;

	Core::logger.AddLogSection( "ProjectConfig" );
	Core::logger.AddLogSection( "LoadFile" );

	Core::logger.AddLogString( LogLevels::ALL, "Loading configuration from file: " + file );

	YAML::Node conf = YAML::LoadFile( file );

	auto v = Vars::GetSingleton();

	pdata.name = v->Replace( GetString( conf, "name" ) );
	pdata.version = v->Replace( GetString( conf, "version" ) );
	pdata.lang = v->Replace( GetString( conf, "lang" ) );
	pdata.std = v->Replace( GetString( conf, "std" ) );
	pdata.compile_flags = v->Replace( GetString( conf, "compile_flags" ) );

	pdata.license = v->Replace( GetString( conf, "license" ) );

	v->AddVar( "license", License::FetchLicenseFormalName( pdata.license ) );

	if( !License::UpdateProjectLicenseFile( pdata.license, update_license ) )
			return Core::ReturnVar( 1 );

	pdata.author.name = v->Replace( GetString( conf, "author", "name" ) );
	pdata.author.email = v->Replace( GetString( conf, "author", "email" ) );

	if( ( pdata.author.name.empty() || pdata.author.email.empty() ) && !this->GetDefaultAuthor() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Configuration load failed. No author information in this file or in system configuration" );
		return Core::ReturnVar( false );
	}

	v->AddVar( "name", pdata.name );
	v->AddVar( "author", pdata.author.name );

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
		build.type = v->Replace( GetString( builddata, "type" ) );
		build.build_type = v->Replace( GetString( builddata, "build_type" ) );
		build.main_src = v->Replace( GetString( builddata, "main_src" ) );

		build.srcs = v->Replace( GetStringVector( builddata, "other_src" ) );
		build.exclude = v->Replace( GetStringVector( builddata, "exclude" ) );

		pdata.builds.push_back( build );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Loaded configuration file successfully" );

	return Core::ReturnVar( true );
}

bool ProjectConfig::SaveFile( const std::string & file )
{
	if( !FS::CreateFile( file ) )
		return false;

	Core::logger.AddLogSection( "ProjectConfig" );
	Core::logger.AddLogSection( "SaveFile" );

	Core::logger.AddLogString( LogLevels::ALL, "Saving configuration for: " + pdata.name + " to file: " + file );

	auto v = Vars::GetSingleton();

	v->AddVar( "name", pdata.name );
	v->AddVar( "author", pdata.author.name );
	v->AddVar( "license", License::FetchLicenseFormalName( pdata.license ) );

	YAML::Emitter o;
	o << YAML::BeginMap;
	o << YAML::Key << "name" << YAML::Value << pdata.name;
	o << YAML::Key << "version" << YAML::Value << pdata.version;
	o << YAML::Key << "lang" << YAML::Value << pdata.lang;
	o << YAML::Key << "std" << YAML::Value << pdata.std;
	o << YAML::Key << "compile_flags" << YAML::Value << pdata.compile_flags;

	o << YAML::Key << "license" << YAML::Value << pdata.license;

	o << YAML::Key << "author" << YAML::Value;
	o << YAML::BeginMap;
	o << YAML::Key << "name" << YAML::Value << pdata.author.name;
	o << YAML::Key << "email" << YAML::Value << pdata.author.email;
	o << YAML::EndMap;

	o << YAML::Key << "libs" << YAML::Value;
	o << YAML::BeginSeq;
	for( auto lib : pdata.libs ) {
		o << YAML::BeginMap;

		o << YAML::Key << "name" << YAML::Value << lib.name;
		o << YAML::Key << "version" << YAML::Value << lib.version;
		o << YAML::Key << "inc_flags" << YAML::Value << lib.inc_flags;
		o << YAML::Key << "lib_flags" << YAML::Value << lib.lib_flags;

		o << YAML::EndMap;
	}
	o << YAML::EndSeq;

	o << YAML::Key << "builds" << YAML::Value;
	o << YAML::BeginSeq;
	for( auto build : pdata.builds ) {
		o << YAML::BeginMap;

		o << YAML::Key << "name" << YAML::Value << build.name;
		o << YAML::Key << "type" << YAML::Value << build.type;
		o << YAML::Key << "main_src" << YAML::Value << build.main_src;
		o << YAML::Key << "other_src" << YAML::Value;
		o << YAML::BeginSeq;
		for( auto src : build.srcs ) {
			o << src;
		}
		o << YAML::EndSeq;

		o << YAML::Key << "exclude" << YAML::Value;
		o << YAML::BeginSeq;
		for( auto exc : build.exclude ) {
			o << exc;
		}
		o << YAML::EndSeq;

		o << YAML::EndMap;
	}
	o << YAML::EndSeq;

	o << YAML::EndMap;

	if( FS::CreateFile( file, o.c_str() ) ) {
		Core::logger.AddLogString( LogLevels::ALL, "Configuration file successfully saved" );
		return Core::ReturnVar( true );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Configuration file save failed" );
	return Core::ReturnVar( false );
}

void ProjectConfig::DisplayAll( const std::string & dir )
{
	Display( "{by}-------------------------------------------------\n\n" );

	Display( "{sc}=> {bm}Author{0}: {bg}" + pdata.author.name + " {0}< {bg}" + pdata.author.email + " {0}>\n" );
	if( !dir.empty() )
		Display( "{sc}=> {bm}Directory{0}: {bg}" + dir + "\n" );
	Display( "{sc}=> {bm}Name{0}: {bg}" + pdata.name + "\n" );
	Display( "{sc}=> {bm}Version{0}: {bg}" + pdata.version + "\n" );
	Display( "{sc}=> {bm}Lang{0}: {bg}" + pdata.lang + "\n" );
	Display( "{sc}=> {bm}Std{0}: {bg}" + pdata.std + "\n" );
	Display( "{sc}=> {bm}Compile_Flags{0}: {bg}" + pdata.compile_flags + "\n\n" );
	Display( "{sc}=> {bm}License{0}: {bg}" + pdata.license + "\n\n" );

	Display( "{sc}=> {bm}Libs{0}:\n" );

	for( auto lib : pdata.libs ) {
		Display( "{sc}===> {bm}Name{0}: {bg}" + lib.name + "\n" );
		Display( "{sc}===> {bm}Version{0}: {bg}" + lib.version + "\n" );
		Display( "{sc}===> {bm}Inc_Flags{0}: {bg}" + lib.inc_flags + "\n" );
		Display( "{sc}===> {bm}Lib_Flags{0}: {bg}" + lib.lib_flags + "\n\n" );
	}

	Display( "\n" );

	Display( "{sc}=> {bm}Builds{0}:\n" );

	for( auto build : pdata.builds ) {
		Display( "{sc}===> {bm}Name{0}: {bg}" + build.name + "\n" );
		Display( "{sc}===> {bm}Type{0}: {bg}" + build.type + "\n" );
		if( build.type == "lib" )
			Display( "{sc}===> {bm}Build Type{0}: {bg}" + build.build_type + "\n" );
		Display( "{sc}===> {bm}Main source{0}: {bg}" + build.main_src + "\n" );
		Display( "{sc}===> {bm}Other sources{0}:\n" );
		for( auto s : build.srcs ) {
			Display( "{sc}=====> {bg}" + s + "\n" );
		}
	}

	Display( "\n{by}--------------------------------------------------{0}");
}