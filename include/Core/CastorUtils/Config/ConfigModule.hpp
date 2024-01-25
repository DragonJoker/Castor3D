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

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <array>
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

//! #define CU_UseWChar

/**@name Configuration */
//@{

/**
*!\~english
*	The macro to use with constant strings, id est : cuT( "a string" ).
*\~french
*	Macro à utiliser pour les chaînes constantes, id est : cuT( "une chaîne").
*/
#if defined( CU_UseWChar )
#define cuT( x ) L##x
namespace castor
{
	using xchar = wchar_t;
}
#else
#define cuT( x ) x
namespace castor
{
	using xchar = char;
}
#endif

namespace castor
{
	using mbchar = char;
	using wchar = wchar_t;
	using u32char = char32_t;

	using String = std::basic_string< xchar >;
	using StringView = std::basic_string_view< xchar >;
	using StringStream = std::basic_stringstream< xchar >;
	using OutputStringStream = std::basic_ostringstream< xchar >;
	using InputStringStream = std::basic_istringstream< xchar >;
	using OutputStream = std::basic_ostream< xchar >;
	using InputStream = std::basic_istream< xchar >;

	using MbString = std::basic_string< mbchar >;
	using MbStringView = std::basic_string_view< mbchar >;
	using MbStringStream = std::basic_stringstream< mbchar >;
	using MbOutputStringStream = std::basic_ostringstream< mbchar >;
	using MbInputStringStream = std::basic_istringstream< mbchar >;
	using MbOutputStream = std::basic_ostream< mbchar >;
	using MbInputStream = std::basic_istream< mbchar >;

	using WString = std::basic_string< wchar >;
	using WStringView = std::basic_string_view< wchar >;
	using WStringStream = std::basic_stringstream< wchar >;
	using WOutputStringStream = std::basic_ostringstream< wchar >;
	using WInputStringStream = std::basic_istringstream< wchar >;
	using WOutputStream = std::basic_ostream< wchar >;
	using WInputStream = std::basic_istream< wchar >;

	using U32String = std::basic_string< u32char >;
	using U32StringView = std::basic_string_view< u32char >;
	using U32StringStream = std::basic_stringstream< u32char >;
	using U32OutputStringStream = std::basic_ostringstream< u32char >;
	using U32InputStringStream = std::basic_istringstream< u32char >;
	using U32OutputStream = std::basic_ostream< u32char >;
	using U32InputStream = std::basic_istream< u32char >;

	template< class FuncT >
	using Function = std::function< FuncT >;

	struct StringHash
	{
		using is_transparent = void;

		std::size_t operator()( std::basic_string_view< char > v )const
		{
			return std::hash< std::basic_string_view< char > >{}( v );
		}

		std::size_t operator()( std::basic_string_view< wchar_t > v )const
		{
			return std::hash< std::basic_string_view< wchar_t > >{}( v );
		}

		std::size_t operator()( std::basic_string_view< char8_t > v )const
		{
			return std::hash< std::basic_string_view< char8_t > >{}( v );
		}

		std::size_t operator()( std::basic_string_view< char16_t > v )const
		{
			return std::hash< std::basic_string_view< char16_t > >{}( v );
		}

		std::size_t operator()( std::basic_string_view< char32_t > v )const
		{
			return std::hash< std::basic_string_view< char32_t > >{}( v );
		}
	};

	template <typename FirstT, typename SecondT >
	using Pair = std::pair< FirstT, SecondT >;
	template< typename KeyT, typename PredT = std::less<>, typename AllocT = std::allocator< KeyT > >
	using Set = std::set< KeyT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< Pair< KeyT const, DataT > > >
	using Map = std::map< KeyT, DataT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< Pair< KeyT const, DataT > > >
	using MultiMap = std::multimap< KeyT, DataT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename PredT = std::less<>, typename AllocT = std::allocator< KeyT > >
	using MultiSet = std::multiset< KeyT, PredT, AllocT >;
	template< typename KeyT, typename DataT, typename HashT = std::hash< KeyT >, typename KeyEqT = std::equal_to<>, typename AllocT = std::allocator< Pair< KeyT const, DataT > > >
	using UnorderedMap = std::unordered_map< KeyT, DataT, HashT, KeyEqT, AllocT >;
	template< typename KeyT, typename HashT = std::hash< KeyT >, typename KeyEqT = std::equal_to<>, typename AllocT = std::allocator< KeyT > >
	using UnorderedSet = std::unordered_set< KeyT, HashT, KeyEqT >;
	template< typename DataT, typename AllocT = std::allocator< DataT > >
	using Vector = std::vector< DataT, AllocT >;
	template< typename DataT, typename AllocT = std::allocator< DataT > >
	using List = std::list< DataT, AllocT >;
	template< typename DataT, size_t SizeT >
	using Array = std::array< DataT, SizeT >;
	template< class DataT, class AllocT = std::allocator< DataT > >
	using Deque = std::deque< DataT, AllocT >;
	template <typename DataT, class ContainerT = Deque< DataT > >
	using Stack = std::stack< DataT, ContainerT >;

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

	CU_DeclareVector( MbString, MbString );
	CU_DeclareVector( String, String );
	CU_DeclareVector( StringView, StringView );
	CU_DeclareVector( MbStringView, MbStringView );
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
		return makeStringStreamT< xchar >();
	}

	CU_API String makeString( MbStringView const & in );
	CU_API String makeString( WStringView const & in );
	CU_API String makeString( U32StringView const & in );

	CU_API MbString toUtf8( WStringView in );
	CU_API WString toSystemWide( MbStringView in );
	CU_API U32String toUtf8U32String( StringView in );

	inline String makeString( MbString const & in )
	{
		return makeString( MbStringView{ in } );
	}

	inline String makeString( WString const & in )
	{
		return makeString( WStringView{ in } );
	}

	inline String makeString( U32String const & in )
	{
		return makeString( U32StringView{ in } );
	}

	inline String makeString( mbchar const * in )
	{
		return makeString( MbStringView{ in } );
	}

	inline String makeString( wchar const * in )
	{
		return makeString( WStringView{ in } );
	}

	inline String makeString( u32char const * in )
	{
		return makeString( U32StringView{ in } );
	}

	inline String makeString( mbchar const * in, size_t length )
	{
		return makeString( MbStringView{ in, length } );
	}

	inline String makeString( wchar const * in, size_t length )
	{
		return makeString( WStringView{ in, length } );
	}

	inline String makeString( u32char const * in, size_t length )
	{
		return makeString( U32StringView{ in, length } );
	}

	inline MbString toUtf8( MbStringView in )
	{
		return MbString{ in };
	}

	inline MbString toUtf8( MbString const & in )
	{
		return in;
	}

	inline MbString toUtf8( mbchar const * in )
	{
		return MbString{ in };
	}

	inline MbString toUtf8( mbchar const * in, size_t length )
	{
		return MbString{ in, length };
	}

	inline MbString toUtf8( WString const & in )
	{
		return toUtf8( WStringView{ in } );
	}

	inline MbString toUtf8( wchar const * in, size_t length )
	{
		return toUtf8( WStringView{ in, length } );
	}

	inline MbString toUtf8( wchar const * in )
	{
		return toUtf8( WStringView{ in } );
	}

	inline WString toSystemWide( MbString const & in )
	{
		return toSystemWide( MbStringView{ in } );
	}

	inline WString toSystemWide( mbchar const * in, size_t length )
	{
		return toSystemWide( MbStringView{ in, length } );
	}

	inline WString toSystemWide( mbchar const * in )
	{
		return toSystemWide( MbStringView{ in } );
	}

	inline WString toSystemWide( WStringView in )
	{
		return WString{ in };
	}

	inline WString toSystemWide( WString const & in )
	{
		return in;
	}

	inline WString toSystemWide( wchar const * in )
	{
		return WString{ in };
	}

	inline WString toSystemWide( wchar const * in, size_t length )
	{
		return WString{ in, length };
	}

	inline U32String toUtf8U32String( String const & in )
	{
		return toUtf8U32String( StringView{ in } );
	}

	inline U32String toUtf8U32String( xchar const * in, size_t length )
	{
		return toUtf8U32String( StringView{ in, length } );
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
