#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#include <codecvt>

#pragma warning( disable: 4273 )

namespace castor::string
{
	bool isInteger( String const & text, CU_UnusedParam( std::locale const &, locale ) )
	{
		bool result = false;

		if ( !text.empty() )
		{
			result = ( text[0] >= '0' && text[0] <= '9' ) || text[0] == '-';

			for ( std::size_t i = 1; i < text.size() && result; i++ )
			{
				result = text[i] >= '0' && text[i] <= '9';
			}
		}

		return result;
	}

	bool isFloating( String const & text, CU_UnusedParam( std::locale const &, locale ) )
	{
		bool result = false;
		StringArray arrayParts;
		String strText( text );
		std::size_t nSize;
		replace( strText, cuT( "," ), cuT( "." ) );
		arrayParts = split( strText, cuT( "." ) );
		nSize = arrayParts.size();

		if ( nSize > 0 && nSize < 3 )
		{
			result = isInteger( arrayParts[0] );

			if ( result && nSize > 1 )
			{
				result = isInteger( arrayParts[1] );
			}
		}

		return result;
	}

	short toShort( String const & text, std::locale const & locale )
	{
		short sReturn = 0;

		if ( !text.empty() )
		{
			parse( text, sReturn, locale );
		}

		return sReturn;
	}

	int toInt( String const & text, std::locale const & locale )
	{
		int iReturn = 0;

		if ( !text.empty() )
		{
			parse( text, iReturn, locale );
		}

		return iReturn;
	}

	long toLong( String const & text, std::locale const & locale )
	{
		long lReturn = 0;

		if ( !text.empty() )
		{
			parse( text, lReturn, locale );
		}

		return lReturn;
	}

	long long toLongLong( String const & text, std::locale const & locale )
	{
		long long llReturn = 0;

		if ( !text.empty() )
		{
			parse( text, llReturn, locale );
		}

		return llReturn;
	}

	unsigned short toUShort( String const & text, std::locale const & locale )
	{
		unsigned short sReturn = 0;

		if ( !text.empty() )
		{
			parse( text, sReturn, locale );
		}

		return sReturn;
	}

	unsigned int toUInt( String const & text, std::locale const & locale )
	{
		unsigned int iReturn = 0;

		if ( !text.empty() )
		{
			parse( text, iReturn, locale );
		}

		return iReturn;
	}

	unsigned long toULong( String const & text, std::locale const & locale )
	{
		unsigned long lReturn = 0;

		if ( !text.empty() )
		{
			parse( text, lReturn, locale );
		}

		return lReturn;
	}

	unsigned long long toULongLong( String const & text, std::locale const & locale )
	{
		unsigned long long llReturn = 0;

		if ( !text.empty() )
		{
			parse( text, llReturn, locale );
		}

		return llReturn;
	}

	float toFloat( String const & text, std::locale const & locale )
	{
		float fReturn = 0;

		if ( !text.empty() )
		{
			parse( text, fReturn, locale );
		}

		return fReturn;
	}

	double toDouble( String const & text, std::locale const & locale )
	{
		double dReturn = 0;

		if ( !text.empty() )
		{
			parse( text, dReturn, locale );
		}

		return dReturn;
	}

	long double toLongDouble( String const & text, std::locale const & locale )
	{
		long double ldReturn = 0;

		if ( !text.empty() )
		{
			parse( text, ldReturn, locale );
		}

		return ldReturn;
	}

	bool isUpperCase( String const & text )
	{
		return text == upperCase( text );
	}

	bool isLowerCase( String const & text )
	{
		return text == lowerCase( text );
	}

	String upperCase( String const & text )
	{
		String strReturn;
		std::locale loc;

		for ( auto c : text )
		{
			strReturn += std::toupper( c, loc );
		}

		return strReturn;
	}

	String lowerCase( String const & text )
	{
		String strReturn;
		std::locale loc;

		for ( auto c : text )
		{
			strReturn += std::tolower( c, loc );
		}

		return strReturn;
	}

	String snakeToCamelCase( String const & str )
	{
		String result;
		result.reserve( str.size() );
		bool prvUnderscore = true;
		std::locale loc;

		for ( auto c : str )
		{
			if ( prvUnderscore )
			{
				result += std::toupper( c, loc );
				prvUnderscore = false;
			}
			else if ( c == '_' )
			{
				prvUnderscore = true;
			}
			else
			{
				result += c;
			}
		}

		if ( prvUnderscore )
		{
			result += '_';
		}

		return result;
	}

	String & toUpperCase( String & text )
	{
		text = upperCase( text );
		return text;
	}

	String & toLowerCase( String & text )
	{
		text = lowerCase( text );
		return text;
	}

	String toString( char32_t value )
	{
		Array< char32_t, 2u > arr{ value, 0 };
		using CharCPtr = char const *;
		return toString( CharCPtr( arr.data() ) );
	}

	String toString( uint8_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( int16_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( uint16_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( int32_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( uint32_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( int64_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	String toString( uint64_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::baseT< xchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( char32_t value )
	{
		Array< char32_t, 2u > arr{ value, 0 };
		using CharCPtr = char const *;
		return toMbString( CharCPtr( arr.data() ) );
	}

	MbString toMbString( uint8_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( int16_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( uint16_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( int32_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( uint32_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( int64_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	MbString toMbString( uint64_t value, int base, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << manip::baseT< mbchar >( base ) << value;
		return result.str();
	}

	bool endsWith( StringView value
		, StringView lookup )
	{
		auto it = value.find( lookup );
		bool result = it != String::npos;

		if ( result )
		{
			result = ( it + lookup.size() ) == value.size();
		}

		return result;
	}

	bool startsWith( StringView value
		, StringView lookup )
	{
		return value.find( lookup ) == 0;
	}
}

namespace castor
{
	namespace convert
	{
		thread_local std::locale const loc{ std::locale{ "C" } };

		template< typename InChar, typename OutChar >
		struct StringConverter
		{
			static void convert( std::basic_string_view< InChar > strIn
				, std::basic_string< OutChar > & strOut )
			{
				if ( !strIn.empty() )
				{
					using facet_type = std::codecvt< OutChar, InChar, std::mbstate_t >;
					auto const & facet = std::use_facet< facet_type >( loc );
					std::mbstate_t state{};
					std::vector< OutChar > dst( strIn.size() * facet.max_length(), 0 );
					const InChar * endSrc = nullptr;
					OutChar * endDst = nullptr;
					facet.in( state
						, strIn.data(), strIn.data() + strIn.size(), endSrc
						, &dst[0], &dst[0] + dst.size(), endDst );
					strOut = std::basic_string< OutChar >( &dst.front(), endDst );
				}
			}
		};

		template<> struct StringConverter< char, wchar_t >
		{
			static void convert( std::basic_string_view< char > strIn
				, std::basic_string< wchar_t > & strOut )
			{
				if ( !strIn.empty() )
				{
					using TmpInCPtr = char8_t const *;
					std::u8string tmpIn{ TmpInCPtr( strIn.data() ), strIn.size() };
					std::u16string tmpOut;
					StringConverter< char8_t, char16_t >::convert( tmpIn, tmpOut );

					for ( auto c : tmpOut )
					{
						strOut += wchar_t( c );
					}
				}
			}
		};

		template<> struct StringConverter< char, char32_t >
		{
			static void convert( std::basic_string_view< char > strIn
				, std::basic_string< char32_t > & strOut )
			{
				if ( !strIn.empty() )
				{
					using TmpInCPtr = char8_t const *;
					std::u8string tmpIn{ TmpInCPtr( strIn.data() ), strIn.size() };
					StringConverter< char8_t, char32_t >::convert( tmpIn, strOut );
				}
			}
		};

		template<> struct StringConverter< wchar_t, char >
		{
			static void convert( std::basic_string_view< wchar_t > strIn
				, std::basic_string< char > & strOut )
			{
				if ( !strIn.empty() )
				{
					for ( auto c : strIn )
					{
						strOut += string::utf8::fromUtf8( char32_t( c ) ).data();
					}
				}
			}
		};

		template<> struct StringConverter< char32_t, char >
		{
			static void convert( std::basic_string_view< char32_t > strIn
				, std::basic_string< char > & strOut )
			{
				if ( !strIn.empty() )
				{
					for ( auto c : strIn )
					{
						strOut += string::utf8::fromUtf8( c ).data();
					}
				}
			}
		};

		template< typename InChar >
		struct StringConverter< InChar, InChar >
		{
			static void convert( std::basic_string_view< InChar > strIn
				, std::basic_string< InChar > & strOut )
			{
				strOut = strIn;
			}
		};
	}

	String makeString( MbStringView const & in )
	{
		String result;
		convert::StringConverter< mbchar, xchar >::convert( in, result );
		return result;
	}

	String makeString( WStringView const & in )
	{
		String result;
		convert::StringConverter< wchar, xchar >::convert( in, result );
		return result;
	}

	String makeString( U32StringView const & in )
	{
		String result;

		if constexpr ( std::is_same_v< xchar, mbchar > )
		{
			MbString tmp;
			convert::StringConverter< char32_t, mbchar >::convert( in, tmp );
			result = makeString( tmp );
		}
		else
		{
			for ( auto c : in )
			{
				result += xchar( c );
			}
		}

		return result;
	}

	MbString toUtf8( WStringView in )
	{
		MbString result;
		convert::StringConverter< wchar, mbchar >::convert( in, result );
		return result;
	}

	WString toSystemWide( MbStringView in )
	{
		WString result;
		convert::StringConverter< mbchar, wchar >::convert( in, result );
		return result;
	}

	U32String toUtf8U32String( StringView in )
	{
		U32String result;

		if constexpr ( std::is_same_v< xchar, mbchar > )
		{
			for ( auto it = string::utf8::cbegin( in ); it != in.end(); ++it )
			{
				result += *it;
			}
		}
		else
		{
			auto tmp = toUtf8( in );
			convert::StringConverter< mbchar, char32_t >::convert( tmp, result );
		}

		return result;
	}
}
