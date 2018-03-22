#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

#include "../include/GlobalData.hpp"
#include "../include/FSFuncs.hpp"
#include "../include/DisplayFuncs.hpp"
#include "../include/Environment.hpp"
#include "../include/UTFChars.hpp"

#include "../include/Core.hpp"

bool Core::Init()
{
	bool new_config = false;

	Global::logger.AddLogSection( "Core" );

	if( !FS::LocExists( Env::CCP4M_DIR ) && !FS::CreateDir( Env::CCP4M_DIR ) ) {
		Display( "{r}Unable to create configuration directory! Please check permissions for your {fc}HOME{r} directory{0}\n" );
		Global::logger.AddLogString( LogLevels::ALL, "Failed to create configuration directory: " + Env::CCP4M_DIR );
		return false;
	}

	if( !FS::LocExists( Env::CCP4M_CONFIG_FILE ) ) {
		if( !FS::CreateFile( Env::CCP4M_CONFIG_FILE ) ) {
			Display( "{r}Unable to create configuration file! Please check permissions for directory: {fc}" + Env::CCP4M_DIR + "{0}\n" );
			return false;
		}
		new_config = true;
	}

	if( !new_config )
		return true;

	Display( "{fc}Initializing first time setup...\n\n" );
	Display( "{sc}Enter your name: {tc}" );

	std::string name;
	std::getline( std::cin, name );

	Display( "{sc}Enter your email: {tc}" );

	std::string email;
	std::getline( std::cin, email );

	Global::logger.AddLogString( LogLevels::ALL, "Config username: " + name + ", email: " + email );

	Display( "\n{fc}Generating system config...\n" );

	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << name << YAML::Comment( "Author name" );
	out << YAML::Key << "email" << YAML::Value << email << YAML::Comment( "Author email" );
	out << YAML::EndMap;

	if( !FS::CreateFile( Env::CCP4M_CONFIG_FILE, out.c_str() ) ) {
		Display( "{fc}Failed to create system config. Please check if you have correct permissions. {br}" + UTF::CROSS + "{0}\n" );
		Global::logger.AddLogString( LogLevels::ALL, "Failed to create system configuration file: " + Env::CCP4M_CONFIG_FILE );
		return false;
	}

	Global::logger.AddLogString( LogLevels::ALL, "Successfully created system configuration file" );

	Global::logger.RemoveLastLogSection();

	Display( "{fc}Successfully generated system config. {bg}" + UTF::TICK + "{0}\n" );
	return true;
}