#include "StringUtils.hpp"

using namespace castor;

namespace castor
{
	namespace string
	{
		bool isInteger( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool result = true;

			if ( ! p_strToTest.empty() )
			{
				result = ( p_strToTest[0] >= '0' && p_strToTest[0] <= '9' ) || p_strToTest[0] == '-';

				for ( std::size_t i = 1; i < p_strToTest.size() && result; i++ )
				{
					result = p_strToTest[i] >= '0' && p_strToTest[i] <= '9';
				}
			}

			return result;
		}

		bool isFloating( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool		result = false;
			StringArray	arrayParts;
			String		strText( p_strToTest );
			std::size_t	nSize;
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

		short toShort( String const & p_strToTest, std::locale const & p_locale )
		{
			short sReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, sReturn, p_locale );
			}

			return sReturn;
		}

		int toInt( String const & p_strToTest, std::locale const & p_locale )
		{
			int iReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, iReturn, p_locale );
			}

			return iReturn;
		}

		long toLong( String const & p_strToTest, std::locale const & p_locale )
		{
			long lReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, lReturn, p_locale );
			}

			return lReturn;
		}

		long long toLongLong( String const & p_strToTest, std::locale const & p_locale )
		{
			long long llReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, llReturn, p_locale );
			}

			return llReturn;
		}

		unsigned short toUShort( String const & p_strToTest, std::locale const & p_locale )
		{
			unsigned short sReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, sReturn, p_locale );
			}

			return sReturn;
		}

		unsigned int toUInt( String const & p_strToTest, std::locale const & p_locale )
		{
			unsigned int iReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, iReturn, p_locale );
			}

			return iReturn;
		}

		unsigned long toULong( String const & p_strToTest, std::locale const & p_locale )
		{
			unsigned long lReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, lReturn, p_locale );
			}

			return lReturn;
		}

		unsigned long long toULongLong( String const & p_strToTest, std::locale const & p_locale )
		{
			unsigned long long llReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, llReturn, p_locale );
			}

			return llReturn;
		}

		float toFloat( String const & p_strToTest, std::locale const & p_locale )
		{
			float fReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, fReturn, p_locale );
			}

			return fReturn;
		}

		double toDouble( String const & p_strToTest, std::locale const & p_locale )
		{
			double dReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, dReturn, p_locale );
			}

			return dReturn;
		}

		long double toLongDouble( String const & p_strToTest, std::locale const & p_locale )
		{
			long double ldReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, ldReturn, p_locale );
			}

			return ldReturn;
		}

		real toReal( String const & p_strToTest, std::locale const & p_locale )
		{
			real rReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, rReturn, p_locale );
			}

			return rReturn;
		}

		bool isUpperCase( String const & p_strToTest )
		{
			return p_strToTest == upperCase( p_strToTest );
		}

		bool isLowerCase( String const & p_strToTest )
		{
			return p_strToTest == lowerCase( p_strToTest );
		}

		String upperCase( String const & p_str )
		{
			String strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < p_str.size(); i++ )
			{
				strReturn += std::toupper( p_str[i], loc );
			}

			return strReturn;
		}

		String lowerCase( String const & p_str )
		{
			String strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < p_str.size(); i++ )
			{
				strReturn += std::tolower( p_str[i], loc );
			}

			return strReturn;
		}

		String & toUpperCase( String & p_str )
		{
			p_str = upperCase( p_str );
			return p_str;
		}

		String & toLowerCase( String & p_str )
		{
			p_str = lowerCase( p_str );
			return p_str;
		}

		StringArray split( String const & p_str, String const & p_delims, uint32_t p_maxSplits, bool p_bKeepVoid )
		{
			StringArray	arrayReturn;

			if ( !p_str.empty() && !p_delims.empty() && p_maxSplits > 0 )
			{
				arrayReturn.reserve( p_maxSplits + 1 );
				std::size_t	pos = 0;
				std::size_t	start = 0;

				do
				{
					pos = p_str.find_first_of( p_delims, start );

					if ( pos == start )
					{
						start = pos + 1;

						if ( p_bKeepVoid )
						{
							arrayReturn.push_back( String() );
						}
					}
					else if ( pos == std::string::npos || arrayReturn.size() == p_maxSplits )
					{
						String remnants = p_str.substr( start );

						if ( !remnants.empty() || p_bKeepVoid )
						{
							arrayReturn.push_back( remnants );
						}

						pos = String::npos;
					}
					else
					{
						arrayReturn.push_back( p_str.substr( start, pos - start ) );
						start = pos + 1;
					}

				}
				while ( pos != std::string::npos );
			}

			return arrayReturn;
		}

		String & trim( String & p_str, bool p_bLeft, bool p_bRight )
		{
			if ( p_str.size() > 0 )
			{
				std::size_t index;

				if ( p_bLeft )
				{
					index = p_str.find_first_not_of( cuT( " \t\r" ) );

					if ( index > 0 )
					{
						if ( index != String::npos )
						{
							p_str = p_str.substr( index, String::npos );
						}
						else
						{
							p_str.clear();
						}
					}
				}

				if ( p_bRight && p_str.size() > 0 )
				{
					index = p_str.find_last_not_of( cuT( " \t\r" ) );

					if ( index < p_str.size() - 1 )
					{
						if ( index != String::npos )
						{
							p_str = p_str.substr( 0, index + 1 );
						}
						else
						{
							p_str.clear();
						}
					}
				}
			}

			return p_str;
		}

		String & replace( String & p_str, xchar p_find, xchar p_replaced )
		{
			xchar szFind[2] = { p_find, cuT( '\0' ) };
			xchar szReplaced[2] = { p_replaced, cuT( '\0' ) };
			return replace( p_str, szFind, szReplaced );
		}

		String & replace( String & p_str, String const & p_find, xchar p_replaced )
		{
			xchar szReplaced[2] = { p_replaced, cuT( '\0' ) };
			return replace( p_str, p_find, szReplaced );
		}

		String & replace( String & p_str, xchar p_find, String const & p_replaced )
		{
			xchar szFind[2] = { p_find, cuT( '\0' ) };
			return replace( p_str, szFind, p_replaced );
		}

		String & replace( String & p_str, String const & p_find, String const & p_replaced )
		{
			String temp;
			String result;
			std::size_t currentPos = 0;
			std::size_t pos = 0;

			while ( ( pos = p_str.find( p_find, currentPos ) ) != String::npos )
			{
				result.append( p_str.substr( currentPos, pos - currentPos ) );
				result.append( p_replaced );
				currentPos = pos + p_find.size();
			}

			if ( currentPos != p_str.size() )
			{
				result.append( p_str.substr( currentPos, pos - currentPos ) );
			}

			p_str = result;
			return p_str;
		}
	}
}
