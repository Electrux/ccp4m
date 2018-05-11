#include <vector>
#include <string>
#include <map>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"

#include "../include/Vars.hpp"

Vars * Vars::v = nullptr;

Vars::Vars()
{
	// Default error string, for when there exists no value for a particular key
	this->vars[ Core::ERR_STR ] = Core::ERR_STR;
}

Vars::~Vars() {}

void Vars::AddVar( const std::string & key, const std::string & val )
{
	this->vars[ key ] = val;
}

std::string Vars::GetVar( const std::string & key )
{
	if( this->vars.find( key ) != this->vars.end() ) return this->vars[ key ];
	return this->vars[ Core::ERR_STR ];
}

Vars * Vars::GetSingleton()
{
	return Vars::v == nullptr ? Vars::v = new Vars : Vars::v;
}

void Vars::DeleteSingleton()
{
	if( Vars::v != nullptr ) {
		delete Vars::v;
		Vars::v = nullptr;
	}
}

void Vars::Initialize()
{
	auto v = GetSingleton();

	// Nothing to do for now :(

	v->AddVar( "CCP4M_DIR", Env::CCP4M_DIR );
	v->AddVar( "CCP4M_CONFIG_FILE", Env::CCP4M_CONFIG_FILE );
	v->AddVar( "CCP4M_LICENSE_DIR", Env::CCP4M_LICENSE_DIR );
	v->AddVar( "CCP4M_LOG_DIR", Env::CCP4M_LOG_DIR );

	v->AddVar( "CCP4M_PROJECT_CONFIG_FILE", Env::CCP4M_PROJECT_CONFIG_FILE );

	Core::logger.AddLogSection( "Vars" );
	Core::logger.AddLogSection( "Initialize" );
	Core::logger.AddLogString( LogLevels::ALL, "Vars initialized successfully" );

	Core::ReturnVoid();
}

// To avoid using static replace function which will cause a lot of overhead if the function is called many times.
int Vars::Replace( std::string & str, bool colors )
{
	int ctr = 0;

	for( auto it = str.begin(); it != str.end(); ) {
		if( * it == '{' ) {
			it = str.erase( it );
			if( it != str.end() && * it == '{' ) {
				++ctr;
				++it;
				continue;
			}

			std::string var;

			while( it != str.end() && * it != '}' ) {
				var += * it;
				it = str.erase( it );
			}

			// Remove the ending brace
			if( it != str.end() ) it = str.erase( it );

			if( var.empty() ) continue;

			std::string val;

			bool found_in_colors = false;

			if( Core::COLORS.find( var ) != Core::COLORS.end() ) found_in_colors = true;

			// Check if it is a part of color. If it is, check if color is enabled. If it is, set that color to val.
			// Otherwise, continue since there is no point of checking in other places ( the use of colors is clearly
			// disabled so just skip it completely, thereby saving log space because of Env::GetEnvVar ( especially in
			// the use of DisplayOneLiner in NW::progress_func( ... ) ) )
			if( colors ) val = Core::COLORS[ var ];
			else if( found_in_colors ) continue;

			if( val.empty() ) val = Env::GetEnvVar( var );
			if( val.empty() ) val = v->GetVar( var );

			if( val == Core::ERR_STR ) continue;

			it = str.insert( it, val.begin(), val.end() );
			it += val.size();
			// Do not add to size if a color is added because it is not a character on terminal
			if( colors && found_in_colors ) continue;

			ctr += val.size();
		}
		else {
			++ctr;
			++it;
		}
	}

	return ctr;
}

std::string Vars::Replace( std::string && str, bool colors )
{
	this->Replace( str, colors );
	return str;
}

std::vector< std::string > Vars::Replace( std::vector< std::string > && vec, bool colors )
{
	for( auto & str : vec ) this->Replace( str, colors );
	return vec;
}

int Vars::ReplaceVars( std::string & str, bool colors )
{
	auto v = GetSingleton();
	return v->Replace( str, colors );
}
