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

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

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
	using u32char = char32_t;
	using String = std::basic_string< xchar >;
	using U32String = std::basic_string< u32char >;
	using StringView = std::basic_string_view< xchar >;
	using U32StringView = std::basic_string_view< u32char >;
	using StringStream = std::basic_stringstream< xchar >;
	using U32StringStream = std::basic_stringstream< u32char >;
	using OutputStringStream = std::basic_ostringstream< xchar >;
	using U32OutputStringStream = std::basic_ostringstream< u32char >;
	using InputStringStream = std::basic_istringstream< xchar >;
	using U32InputStringStream = std::basic_istringstream< u32char >;
	using OutputStream = std::basic_ostream< xchar >;
	using U32OutputStream = std::basic_ostream< u32char >;
	using InputStream = std::basic_istream< xchar >;
	using U32InputStream = std::basic_istream< u32char >;

	struct StringHash
	{
		using is_transparent = void;

		std::size_t operator()( std::string_view v )const
		{
			return std::hash< std::string_view >{}( v );
		}

		std::size_t operator()( std::wstring_view v )const
		{
			return std::hash< std::wstring_view >{}( v );
		}

		std::size_t operator()( U32StringView v )const
		{
			return std::hash< U32StringView >{}( v );
		}
	};

	template< typename KeyT, typename PredT = std::less<>, typename AllocT = std::allocator< KeyT > >
	using Set = std::set< KeyT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< std::pair< KeyT const, DataT > > >
	using Map = std::map< KeyT, DataT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< std::pair< KeyT const, DataT > > >
	using MultiMap = std::multimap< KeyT, DataT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< KeyT > >
	using MultiSet = std::multiset< KeyT, PredT, AllocT >;
	template< typename DataT, typename AllocT = std::allocator< DataT > >
	using Vector = std::vector< DataT, AllocT >;
	template< typename DataT, typename AllocT = std::allocator< DataT > >
	using List = std::list< DataT, AllocT >;
	template< typename DataT, size_t SizeT >
	using Array = std::array< DataT, SizeT >;

	template< typename DataT >
	using StringMap = Map< String, DataT >;
	template< typename DataT >
	using UnorderedStringMap = std::unordered_map< String, DataT, StringHash, std::equal_to<> >;

	using StringSet = std::set< String, StringHash, std::equal_to<> >;
	using UnorderedStringSet = std::unordered_set< String, StringHash, std::equal_to<> >;

	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
	using Microseconds = std::chrono::microseconds;
	using Nanoseconds = std::chrono::nanoseconds;

	CU_DeclareVector( String, String );
	CU_DeclareVector( StringView, StringView );
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
