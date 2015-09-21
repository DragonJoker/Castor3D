#include "StringUtils.hpp"

#include "Exception.hpp"

#include <locale>
#include <wchar.h>
#include <cstdlib>
#include <cstring>

using namespace Castor;

namespace Castor
{
	namespace string
	{
		bool is_integer( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool l_return = true;

			if ( ! p_strToTest.empty() )
			{
				l_return = ( p_strToTest[0] >= '0' && p_strToTest[0] <= '9' ) || p_strToTest[0] == '-';

				for ( std::size_t i = 1; i < p_strToTest.size() && l_return; i++ )
				{
					l_return = p_strToTest[i] >= '0' && p_strToTest[i] <= '9';
				}
			}

			return l_return;
		}

		bool is_floating( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool		l_return = false;
			StringArray	l_arrayParts;
			String		l_strText( p_strToTest );
			std::size_t	l_nSize;
			replace( l_strText, cuT( "," ), cuT( "." ) );
			l_arrayParts = split( l_strText, cuT( "." ) );
			l_nSize = l_arrayParts.size();

			if ( l_nSize > 0 && l_nSize < 3 )
			{
				l_return = is_integer( l_arrayParts[0] );

				if ( l_return && l_nSize > 1 )
				{
					l_return = is_integer( l_arrayParts[1] );
				}
			}

			return l_return;
		}

		short to_short( String const & p_strToTest, std::locale const & p_locale )
		{
			short l_sReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_sReturn, p_locale );
			}

			return l_sReturn;
		}

		int to_int( String const & p_strToTest, std::locale const & p_locale )
		{
			int l_iReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_iReturn, p_locale );
			}

			return l_iReturn;
		}

		long to_long( String const & p_strToTest, std::locale const & p_locale )
		{
			long l_lReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_lReturn, p_locale );
			}

			return l_lReturn;
		}

		long long to_long_long( String const & p_strToTest, std::locale const & p_locale )
		{
			long long l_llReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_llReturn, p_locale );
			}

			return l_llReturn;
		}

		float to_float( String const & p_strToTest, std::locale const & p_locale )
		{
			float l_fReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_fReturn, p_locale );
			}

			return l_fReturn;
		}

		double to_double( String const & p_strToTest, std::locale const & p_locale )
		{
			double l_dReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_dReturn, p_locale );
			}

			return l_dReturn;
		}

		long double to_long_double( String const & p_strToTest, std::locale const & p_locale )
		{
			long double l_ldReturn = 0;

			if ( !p_strToTest.empty() )
			{
				parse( p_strToTest, l_ldReturn, p_locale );
			}

			return l_ldReturn;
		}

		real to_real( String const & p_strToTest, std::locale const & p_locale )
		{
			real l_rReturn = 0;

			if ( ! p_strToTest.empty() )
			{
				parse( p_strToTest, l_rReturn, p_locale );
			}

			return l_rReturn;
		}

		bool is_upper_case( String const & p_strToTest )
		{
			return p_strToTest == upper_case( p_strToTest );
		}

		bool is_lower_case( String const & p_strToTest )
		{
			return p_strToTest == lower_case( p_strToTest );
		}

		String upper_case( String const & p_str )
		{
			String l_strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < p_str.size(); i++ )
			{
				l_strReturn += std::toupper( p_str[i], loc );
			}

			return l_strReturn;
		}

		String lower_case( String const & p_str )
		{
			String l_strReturn;
			std::locale loc;

			for ( std::size_t i = 0; i < p_str.size(); i++ )
			{
				l_strReturn += std::tolower( p_str[i], loc );
			}

			return l_strReturn;
		}

		String & to_upper_case( String & p_str )
		{
			p_str = upper_case( p_str );
			return p_str;
		}

		String & to_lower_case( String & p_str )
		{
			p_str = lower_case( p_str );
			return p_str;
		}

		StringArray split( String const & p_str, String const & p_delims, uint32_t p_maxSplits, bool p_bKeepVoid )
		{
			StringArray	l_arrayReturn;

			if ( !p_str.empty() && !p_delims.empty() && p_maxSplits > 0 )
			{
				l_arrayReturn.reserve( p_maxSplits + 1 );
				std::size_t	l_pos = 0;
				std::size_t	l_start = 0;

				do
				{
					l_pos = p_str.find_first_of( p_delims, l_start );

					if ( l_pos == l_start )
					{
						l_start = l_pos + 1;

						if ( p_bKeepVoid )
						{
							l_arrayReturn.push_back( String() );
						}
					}
					else if ( l_pos == std::string::npos || l_arrayReturn.size() == p_maxSplits )
					{
						String remnants = p_str.substr( l_start );

						if ( !remnants.empty() || p_bKeepVoid )
						{
							l_arrayReturn.push_back( remnants );
						}

						l_pos = String::npos;
					}
					else
					{
						l_arrayReturn.push_back( p_str.substr( l_start, l_pos - l_start ) );
						l_start = l_pos + 1;
					}

				}
				while ( l_pos != std::string::npos );
			}

			return l_arrayReturn;
		}

		String & trim( String & p_str, bool p_bLeft, bool p_bRight )
		{
			if ( p_str.size() > 0 )
			{
				std::size_t l_uiIndex;

				if ( p_bLeft )
				{
					l_uiIndex = p_str.find_first_not_of( cuT( ' ' ) );

					if ( l_uiIndex > 0 )
					{
						if ( l_uiIndex != String::npos )
						{
							p_str = p_str.substr( l_uiIndex, String::npos );
						}
						else
						{
							p_str.clear();
						}
					}

					l_uiIndex = p_str.find_first_not_of( cuT( '\t' ) );

					if ( l_uiIndex > 0 )
					{
						if ( l_uiIndex != String::npos )
						{
							p_str = p_str.substr( l_uiIndex, String::npos );
						}
						else
						{
							p_str.clear();
						}
					}
				}

				if ( p_bRight && p_str.size() > 0 )
				{
					l_uiIndex = p_str.find_last_not_of( cuT( ' ' ) );

					if ( l_uiIndex < p_str.size() - 1 )
					{
						if ( l_uiIndex != String::npos )
						{
							p_str = p_str.substr( 0, l_uiIndex + 1 );
						}
						else
						{
							p_str.clear();
						}
					}

					if ( p_str.size() > 0 )
					{
						l_uiIndex = p_str.find_last_not_of( cuT( '\t' ) );

						if ( l_uiIndex != String::npos )
						{
							p_str = p_str.substr( 0, l_uiIndex + 1 );
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
			xchar l_szFind[2] = { p_find, cuT( '\0' ) };
			xchar l_szReplaced[2] = { p_replaced, cuT( '\0' ) };
			return replace( p_str, l_szFind, l_szReplaced );
		}

		String & replace( String & p_str, String const & p_find, xchar p_replaced )
		{
			xchar l_szReplaced[2] = { p_replaced, cuT( '\0' ) };
			return replace( p_str, p_find, l_szReplaced );
		}

		String & replace( String & p_str, xchar p_find, String const & p_replaced )
		{
			xchar l_szFind[2] = { p_find, cuT( '\0' ) };
			return replace( p_str, l_szFind, p_replaced );
		}

		String & replace( String & p_str, String const & p_find, String const & p_replaced )
		{
			String		l_temp;
			String		l_return;
			std::size_t	l_currentPos	= 0;
			std::size_t	l_pos			= 0;

			while ( ( l_pos = p_str.find( p_find, l_currentPos ) ) != String::npos )
			{
				l_return.append( p_str.substr( l_currentPos, l_pos - l_currentPos ) );
				l_return.append( p_replaced );
				l_currentPos = l_pos + p_find.size();
			}

			if ( l_currentPos != p_str.size() )
			{
				l_return.append( p_str.substr( l_currentPos, l_pos - l_currentPos ) );
			}

			p_str = l_return;
			return p_str;
		}
	}
}
