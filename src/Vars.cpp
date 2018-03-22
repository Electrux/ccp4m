#include <string>
#include <map>
#include <iostream>

#include "../include/GlobalData.hpp"
#include "../include/Colors.hpp"
#include "../include/Environment.hpp"

#include "../include/Vars.hpp"

Vars * Vars::v = nullptr;

Vars::Vars()
{
	// Default error string, for when there exists no value for a particular key
	this->vars[ Global::ERR_STR ] = Global::ERR_STR;
}

Vars::~Vars() {}

void Vars::AddVar( const std::string & key, const std::string & val )
{
	this->vars[ key ] = val;
}

std::string Vars::GetVar( const std::string & key )
{
	if( this->vars.find( key ) != this->vars.end() )
		return this->vars[ key ];

	return this->vars[ Global::ERR_STR ];
}

Vars * Vars::GetSingleton()
{
	if( Vars::v == nullptr )
		Vars::v = new Vars;

	return Vars::v;
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

	v->AddVar( "CCP4M_DIR", Env::CCP4M_DIR );
	v->AddVar( "CCP4M_CONFIG_FILE", Env::CCP4M_CONFIG_FILE );
}

// To avoid using static replace function which will cause a lot of overhead if the function is called many times.
int Vars::Replace( std::string & str, bool colors )
{
	int ctr = 0;

	for( auto it = str.begin(); it != str.end(); ++it ) {
		if( * it == '{' ) {
			it = str.erase( it );
			if( it != str.end() && * it == '{' ) {
				++ctr;
				continue;
			}

			std::string var;

			while( it != str.end() && * it != '}' ) {
				var += * it;
				it = str.erase( it );
			}

			// Remove the ending brace
			if( it != str.end() )
				it = str.erase( it );

			std::string val;

			if( colors && COLORS.find( var ) != COLORS.end() )
				val = COLORS[ var ];
			
			if( val.empty() )
				val = Env::GetEnvVar( var );

			if( val.empty() )
				val = v->GetVar( var );

			if( val == Global::ERR_STR )
				continue;

			it = str.insert( it, val.begin(), val.end() );
			it += val.size() - 1;
			ctr += val.size();
		}
		else {
			++ctr;
		}
	}

	return ctr;
}

std::string Vars::Replace( std::string && str )
{
	this->Replace( str );
	return str;
}

std::vector< std::string > Vars::Replace( std::vector< std::string > && vec )
{
	for( auto & str : vec )
		this->Replace( str );

	return vec;
}

int Vars::ReplaceVars( std::string & str, bool colors )
{
	auto v = GetSingleton();
	return v->Replace( str, colors );
}