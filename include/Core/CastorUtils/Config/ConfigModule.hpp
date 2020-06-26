/*
See LICENSE file in root folder
*/
#ifndef ___CU_ConfigModule_HPP___
#define ___CU_ConfigModule_HPP___
#pragma once

#include "CastorUtils/config.hpp"
#include "CastorUtils/Config/CompilerConfig.hpp"
#include "CastorUtils/Config/Macros.hpp"
#include "CastorUtils/Config/MultiThreadConfig.hpp"
#include "CastorUtils/Config/PlatformConfig.hpp"
#include "CastorUtils/Config/SmartPtr.hpp"

#include <chrono>
#include <string>
#include <iostream>
#include <sstream>

/**@name Configuration */
//@{

/**
*!\~english
*	The macro to use with constant strings, id est : cuT( "a string" ).
*\~french
*	Macro à utiliser pour les chaînes constantes, id est : cuT( "une chaîne").
*/
#define cuT( x ) x

namespace castor
{
	using xchar = char;
	using ychar = wchar_t;
	using String = std::basic_string< xchar >;
	using StringStream = std::basic_stringstream< xchar >;
	using OutputStringStream = std::basic_ostringstream< xchar >;
	using InputStringStream = std::basic_istringstream< xchar >;
	using OutputStream = std::basic_ostream< xchar >;
	using InputStream = std::basic_istream< xchar >;

	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;

	CU_DeclareVector( String, String );
	CU_DeclareMap( String, uint32_t, UInt32Str );
	CU_DeclareMap( String, uint64_t, UInt64Str );
	CU_DeclareMap( String, bool, BoolStr );
	CU_DeclareMap( String, String, StrStr );
	CU_DeclareSet( String, Str );
	CU_DeclareMap( uint32_t, String, StrUInt32 );
	/**
	 *\~english
	 *\return		A std::stringstream using C locale.
	 *\~french
	 *\return		Un std::stringstream utilisant une locale C.
	 */
	template< typename CharT >
	inline std::basic_stringstream< CharT > makeStringStreamT()
	{
		static std::locale const loc{ "C" };
		std::basic_stringstream< CharT > result;
		result.imbue( loc );
		return result;
	}
	/**
	 *\~english
	 *\return		A std::stringstream using C locale.
	 *\~french
	 *\return		Un std::stringstream utilisant une locale C.
	 */
	inline StringStream makeStringStream()
	{
		static std::locale const loc{ "C" };
		StringStream result;
		result.imbue( loc );
		return result;
	}
}

constexpr castor::Seconds operator "" _s( unsigned long long value )
{
	return castor::Seconds( int64_t( value ) );
}

constexpr castor::Milliseconds operator "" _ms( unsigned long long value )
{
	return castor::Milliseconds( int64_t( value ) );
}

constexpr castor::Microseconds operator "" _us( unsigned long long value )
{
	return castor::Microseconds( int64_t( value ) );
}

constexpr castor::Nanoseconds operator "" _ns( unsigned long long value )
{
	return castor::Nanoseconds( int64_t( value ) );
}
//@}

#endif
