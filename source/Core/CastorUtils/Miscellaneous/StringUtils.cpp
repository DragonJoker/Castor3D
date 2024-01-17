#include "CastorUtils/Miscellaneous/StringUtils.hpp"

#pragma warning( disable: 4273 )

namespace castor::string
{
	namespace helpers
	{
		template< typename StringT >
		std::vector< StringT > split( StringT const & text
			, StringView delims
			, uint32_t maxSplits
			, bool keepEmpty )
		{
			std::vector< StringT >	result;

			if ( !text.empty() && !delims.empty() && maxSplits > 0 )
			{
				result.reserve( maxSplits + 1 );
				std::size_t pos = 0;
				std::size_t start = 0;

				do
				{
					pos = text.find_first_of( delims, start );

					if ( pos == start )
					{
						start = pos + 1;

						if ( keepEmpty )
						{
							result.emplace_back();
						}
					}
					else if ( pos == std::string::npos || result.size() == maxSplits )
					{
						if ( StringT remnants = text.substr( start );
							!remnants.empty() || keepEmpty )
						{
							result.push_back( remnants );
						}

						pos = String::npos;
					}
					else
					{
						result.push_back( text.substr( start, pos - start ) );
						start = pos + 1;
					}

				}
				while ( pos != std::string::npos );
			}

			return result;
		}

		template< typename StringT >
		StringT & trim( StringT & text
			, bool left
			, bool right
			, StringView seps )
		{
			if ( !text.empty() )
			{
				if ( left )
				{
					if ( auto index = text.find_first_not_of( seps );
						index > 0 )
					{
						if ( index != String::npos )
						{
							text = text.substr( index, String::npos );
						}
						else
						{
							text = text.substr( 0, 0 );
						}
					}
				}

				if ( right && !text.empty() )
				{
					if ( auto index = text.find_last_not_of( seps );
						index < text.size() - 1 )
					{
						if ( index != String::npos )
						{
							text = text.substr( 0, index + 1 );
						}
						else
						{
							text = text.substr( 0, 0 );
						}
					}
				}
			}

			return text;
		}
	}

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

	StringArray split( String const & text
		, StringView delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return helpers::split( text, delims, maxSplits, keepEmpty );
	}

	StringViewArray split( StringView text
		, StringView delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return helpers::split( text, delims, maxSplits, keepEmpty );
	}

	String & trim( String & text
		, bool left
		, bool right
		, StringView seps )
	{
		return helpers::trim( text, left, right, seps );
	}

	StringView & trim( StringView & text
		, bool left
		, bool right
		, StringView seps )
	{
		return helpers::trim( text, left, right, seps );
	}

	String toString( char32_t value )
	{
		std::array< char32_t, 2u > arr{ value, 0 };
		return toString( reinterpret_cast< char const * >( arr.data() ) );
	}

	String toString( uint8_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( int16_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( uint16_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( int32_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( uint32_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( int64_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	String toString( uint64_t value, int base, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << manip::xbase( base ) << value;
		return result.str();
	}

	U32String toU32String( String const & text )
	{
		U32String result;

		for ( string::utf8::const_iterator it{ text.begin() }; it != text.end(); ++it )
		{
			result += *it;
		}

		return result;
	}

	String & replace( String & text, xchar lookup, xchar replacement )
	{
		return replace( text
			, String{ lookup, cuT( '\0' ) }
			, String{ replacement, cuT( '\0' ) } );
	}

	String & replace( String & text, StringView lookup, xchar replacement )
	{
		return replace( text
			, lookup
			, String{ replacement, cuT( '\0' ) } );
	}

	String & replace( String & text, xchar lookup, StringView replacement )
	{
		return replace( text
			, String{ lookup, cuT( '\0' ) }
			, replacement );
	}

	String & replace( String & text, StringView lookup, StringView replacement )
	{
		String result;
		std::size_t currentPos = 0;
		std::size_t pos = 0;

		while ( ( pos = text.find( lookup, currentPos ) ) != String::npos )
		{
			result.append( text.substr( currentPos, pos - currentPos ) );
			result.append( replacement );
			currentPos = pos + lookup.size();
		}

		if ( currentPos != text.size() )
		{
			result.append( text.substr( currentPos, pos - currentPos ) );
		}

		text = result;
		return text;
	}

	String getLongestCommonSubstring( String const & lhs
		, String const & rhs )
	{
		if ( lhs.empty() )
		{
			return rhs;
		}

		if ( rhs.empty() )
		{
			return lhs;
		}

		std::vector< String > substrings;

		for ( auto beg = lhs.begin(); beg != std::prev( lhs.end() ); ++beg )
		{
			for ( auto end = beg; end != lhs.end(); ++end )
			{
				if ( rhs.find( String{ beg, std::next( end ) } ) != String::npos )
				{
					substrings.emplace_back( beg, std::next( end ) );
				}
			}
		}

		if ( substrings.empty() )
		{
			return castor::String{};
		}

		return *std::max_element( substrings.begin(), substrings.end(),
			[]( auto & elem1, auto & elem2 )
			{
				return elem1.length() < elem2.length();
			} );
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
