#include <vector>
#include <string>
#include <map>

#include "../include/Core.hpp"
#include "../include/Environment.hpp"
#include "../include/DisplayFuncs.hpp"

#include "../include/StringFuncs.hpp"

std::vector< std::string > Str::DoublePtrToVector( const int argc, const char ** & argv )
{
	std::vector< std::string > vec;

	for( int i = 0; i < argc; ++i )
		vec.push_back( argv[ i ] );

	return vec;
}

std::vector< std::string > Str::Delimit( const std::string & str, const char ch )
{
	std::string temp;
	std::vector< std::string > vec;

	for( auto c : str ) {
		if( c == ' ' && ch != ' ' )
			continue;

		if( c == ch ) {
			if( temp.empty() )
				continue;
			vec.push_back( temp );
			temp.clear();
			continue;
		}

		temp += c;
	}

	if( !temp.empty() )
		vec.push_back( temp );

	return vec;
}

std::string Str::ToLower( const std::string & str )
{
	std::string res;
	for( auto c : str ) {
		res += std::tolower( c );
	}

	return res;
}

std::string Str::ToUpper( const std::string & str )
{
	std::string res;
	for( auto c : str ) {
		res += std::toupper( c );
	}

	return res;
}

int Str::FetchArgs( const std::vector< std::string > & args, std::map< std::string, std::string > & argmap, int from )
{
	Core::logger.AddLogSection( "Project" );
	Core::logger.AddLogSection( "FetchArgs" );

	if( args.size() == from )
		return Core::ReturnVar( 0 );

	if( args.size() < from ) {
		Core::logger.AddLogString( LogLevels::ALL, "Argument size is: " + std::to_string( args.size() ) + " but initialize point is: " + std::to_string( from ) );
		Display( "{fc}Not enough parameters provided{0}, refer to the program\'s usage{0}\n" );
		return Core::ReturnVar( 2 );
	}

	bool expect_arg = false;
	std::string key;

	argmap.clear();

	for( auto arg = args.begin() + from; arg != args.end(); ++arg ) {
		if( arg->substr( 0, 2 ) == "--" ) {
			if( !key.empty() ) {
				Core::logger.AddLogString( LogLevels::ALL, "Invalid syntax used - Consecutive parameters without value in between" );
				Display( "{fc}Invalid argument syntax used{0}: {fc}Attempted to have multiple consecutive parameters without having a value in between them{0}\n" );
				Display( "{fc}Faulty parameters are{0}: {r}" + key + " {fc}and{0}: {r}" + arg->substr( 2 ) + "{0}\n" );
				return Core::ReturnVar( 1 );
			}
			expect_arg = true;
			key = arg->substr( 2 );
			continue;
		}

		if( key.empty() ) {
			Core::logger.AddLogString( LogLevels::ALL, "Invalid syntax used - Attempted to add a parameter value without previously defining parameter" );
			Display( "{fc}Invalid argument syntax used{0}: {fc}Attempted to add a parameter value{0}: {r}" + * arg + " {fc}without previously defining parameter\n" );
			return Core::ReturnVar( 1 );
		}

		argmap[ key ] = * arg;
		Core::logger.AddLogString( LogLevels::ALL, "Adding argument: " + key + " with value: " + * arg );
		key.clear();
	}

	if( !key.empty() ) {
		Core::logger.AddLogString( LogLevels::ALL, "Invalid syntax used - A Parameter does not have any value associated with it" );
		Display( "{fc}Invalid argument syntax used{0}: {fc}Parameter{0}: {r}" + key + " {fc}does not have any value associated with it{0}\n" );
		return Core::ReturnVar( 1 );
	}

	Core::logger.AddLogString( LogLevels::ALL, "Successfully fetched " + std::to_string( argmap.size() ) + " arguments" );
	return Core::ReturnVar( 0 );
}