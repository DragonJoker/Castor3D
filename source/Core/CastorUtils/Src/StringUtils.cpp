#include "StringUtils.hpp"
#include <locale>
#include <wchar.h>
#include <cstdlib>
#include <cstring>

using namespace Castor;

namespace Castor
{
	namespace str_utils
	{
		namespace
		{
			int DoGetMonthIndex( String const & p_strMonth )
			{
				int l_iReturn = 0;
				static std::map< String, int > l_mapMonths;

				if ( l_mapMonths.empty() )
				{
					l_mapMonths.insert( std::make_pair( cuT( "janvier"	),	 1 ) );
					l_mapMonths.insert( std::make_pair( cuT( "février"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "fevrier"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "mars"	),	 3 ) );
					l_mapMonths.insert( std::make_pair( cuT( "avril"	),	 4 ) );
					l_mapMonths.insert( std::make_pair( cuT( "mai"	),	 5 ) );
					l_mapMonths.insert( std::make_pair( cuT( "juin"	),	 6 ) );
					l_mapMonths.insert( std::make_pair( cuT( "juillet"	),	 7 ) );
					l_mapMonths.insert( std::make_pair( cuT( "août"	),	 8 ) );
					l_mapMonths.insert( std::make_pair( cuT( "aout"	),	 8 ) );
					l_mapMonths.insert( std::make_pair( cuT( "septembre"	),	 9 ) );
					l_mapMonths.insert( std::make_pair( cuT( "octobre"	),	10 ) );
					l_mapMonths.insert( std::make_pair( cuT( "novembre"	),	11 ) );
					l_mapMonths.insert( std::make_pair( cuT( "décembre"	),	12 ) );
					l_mapMonths.insert( std::make_pair( cuT( "decembre"	),	12 ) );
					l_mapMonths.insert( std::make_pair( cuT( "jan"	),	 1 ) );
					l_mapMonths.insert( std::make_pair( cuT( "fév"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "fev"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "mar"	),	 3 ) );
					l_mapMonths.insert( std::make_pair( cuT( "avr"	),	 4 ) );
					l_mapMonths.insert( std::make_pair( cuT( "mai"	),	 5 ) );
					l_mapMonths.insert( std::make_pair( cuT( "jun"	),	 6 ) );
					l_mapMonths.insert( std::make_pair( cuT( "jul"	),	 7 ) );
					l_mapMonths.insert( std::make_pair( cuT( "aoû"	),	 8 ) );
					l_mapMonths.insert( std::make_pair( cuT( "aou"	),	 8 ) );
					l_mapMonths.insert( std::make_pair( cuT( "sep"	),	 9 ) );
					l_mapMonths.insert( std::make_pair( cuT( "oct"	),	10 ) );
					l_mapMonths.insert( std::make_pair( cuT( "nov"	),	11 ) );
					l_mapMonths.insert( std::make_pair( cuT( "déc"	),	12 ) );
					l_mapMonths.insert( std::make_pair( cuT( "dec"	),	12 ) );
					l_mapMonths.insert( std::make_pair( cuT( "january"	),	 1 ) );
					l_mapMonths.insert( std::make_pair( cuT( "february"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "march"	),	 3 ) );
					l_mapMonths.insert( std::make_pair( cuT( "april"	),	 4 ) );
					l_mapMonths.insert( std::make_pair( cuT( "may"	),	 5 ) );
					l_mapMonths.insert( std::make_pair( cuT( "june"	),	 6 ) );
					l_mapMonths.insert( std::make_pair( cuT( "july"	),	 7 ) );
					l_mapMonths.insert( std::make_pair( cuT( "august"	),	 8 ) );
					l_mapMonths.insert( std::make_pair( cuT( "september"	),	 9 ) );
					l_mapMonths.insert( std::make_pair( cuT( "october"	),	10 ) );
					l_mapMonths.insert( std::make_pair( cuT( "november"	),	11 ) );
					l_mapMonths.insert( std::make_pair( cuT( "december"	),	12 ) );
					l_mapMonths.insert( std::make_pair( cuT( "feb"	),	 2 ) );
					l_mapMonths.insert( std::make_pair( cuT( "apr"	),	 4 ) );
					l_mapMonths.insert( std::make_pair( cuT( "may"	),	 5 ) );
					l_mapMonths.insert( std::make_pair( cuT( "aug"	),	 8 ) );
				}

				if ( is_integer( p_strMonth ) )
				{
					l_iReturn = to_int( p_strMonth );
				}
				else
				{
					String l_strMonthLC = p_strMonth;
					std::map< String, int >::const_iterator l_it = l_mapMonths.find( to_lower_case( l_strMonthLC ) );

					if ( l_it != l_mapMonths.end() )
					{
						l_iReturn = l_it->second;
					}
				}

				return l_iReturn;
			}

			bool DoIsValidDay( int p_iDay, int p_iMonth, int p_iYear )
			{
				bool l_bReturn = false;

				if ( p_iDay > 0 )
				{
					if ( p_iMonth == 1 || p_iMonth == 3 || p_iMonth == 5 || p_iMonth == 7 || p_iMonth == 8 || p_iMonth == 10 || p_iMonth == 12 )
					{
						l_bReturn = p_iDay <= 31;
					}
					else if ( p_iMonth != 2 )
					{
						l_bReturn = p_iDay <= 30;
					}
					else if ( p_iYear % 400 == 0 || ( p_iYear % 4 == 0 && p_iYear % 100 != 0 ) )
					{
						l_bReturn = p_iDay <= 29;
					}
					else
					{
						l_bReturn = p_iDay <= 28;
					}
				}

				return l_bReturn;
			}

			template< typename InChar, typename OutChar > struct str_converter;

			template<> struct str_converter< char, wchar_t >
			{
				void operator()( std::basic_string< char > const & p_strIn, std::basic_string< wchar_t > & p_strOut, std::locale const & p_locale = std::locale() )
				{
					if ( !p_strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						typedef facet_type::result result_type;
						const facet_type & facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t state = std::mbstate_t();
						std::vector< wchar_t > dst( p_strIn.size() * facet.max_length(), 0 );
						const char * endSrc = NULL;
						wchar_t * endDst = NULL;
						facet.in( state,
								  p_strIn.data(), p_strIn.data() + p_strIn.size(), endSrc,
								  &dst[0], &dst[0] + dst.size(), endDst
								);
						p_strOut = std::wstring( &dst.front(), endDst );
					}
				}
			};

			template<> struct str_converter< wchar_t, char >
			{
				void operator()( std::basic_string< wchar_t > const & p_strIn, std::basic_string< char > & p_strOut, std::locale const & p_locale = std::locale() )
				{
					if ( !p_strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						typedef facet_type::result result_type;
						const facet_type & facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t state = std::mbstate_t();
						std::vector< char > dst( p_strIn.size() * facet.max_length(), 0 );
						const wchar_t * endSrc = NULL;
						char * endDst = NULL;
						facet.out( state,
								   p_strIn.data(), p_strIn.data() + p_strIn.size(), endSrc,
								   &dst[0], &dst[0] + dst.size(), endDst
								 );
						p_strOut = std::string( &dst.front(), endDst );
					}
				}
			};

			template< typename InChar >
			struct str_converter< InChar, InChar >
			{
				void operator()( std::basic_string< InChar > const & p_strIn, std::basic_string< InChar > & p_strOut, std::locale const & )
				{
					p_strOut = p_strIn;
				}
			};
		}

		//*************************************************************************************************

		bool is_integer( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool l_bReturn = true;

			if ( ! p_strToTest.empty() )
			{
				l_bReturn = ( p_strToTest[0] >= '0' && p_strToTest[0] <= '9' ) || p_strToTest[0] == '-';

				for ( std::size_t i = 1; i < p_strToTest.size() && l_bReturn; i++ )
				{
					l_bReturn = p_strToTest[i] >= '0' && p_strToTest[i] <= '9';
				}
			}

			return l_bReturn;
		}

		bool is_floating( String const & p_strToTest, std::locale const & CU_PARAM_UNUSED( p_locale ) )
		{
			bool		l_bReturn = false;
			StringArray	l_arrayParts;
			String		l_strText( p_strToTest );
			std::size_t	l_nSize;
			replace( l_strText, cuT( "," ), cuT( "." ) );
			l_arrayParts = split( l_strText, cuT( "." ) );
			l_nSize = l_arrayParts.size();

			if ( l_nSize > 0 && l_nSize < 3 )
			{
				l_bReturn = is_integer( l_arrayParts[0] );

				if ( l_bReturn && l_nSize > 1 )
				{
					l_bReturn = is_integer( l_arrayParts[1] );
				}
			}

			return l_bReturn;
		}

		bool is_date( String const & p_strToTest, std::locale const & p_locale )
		{
			bool l_bReturn = false;
			String l_strText( p_strToTest );
			String l_strYear;
			String l_strMonth;
			String l_strDay;
			std::tm l_tmbuf = { 0 };
			std::time_get<xchar> const & l_timeFacet =  std::use_facet< std::time_get< xchar > >( p_locale );
			std::time_get<xchar>::dateorder l_order = l_timeFacet.date_order();
			StringArray l_arraySplitted;
			replace( l_strText, cuT( "/" ), cuT( " " ) );
			replace( l_strText, cuT( "-" ), cuT( " " ) );

			while ( l_strText.find( cuT( "  " ) ) != String::npos )
			{
				replace( l_strText, cuT( "  " ), cuT( " " ) );
			}

			l_arraySplitted = split( l_strText, cuT( " " ), 4, false );

			if ( l_arraySplitted.size() == 3 && is_integer( l_arraySplitted[0] ) && is_integer( l_arraySplitted[2] ) )
			{
				l_bReturn = true;

				switch ( l_order )
				{
				case std::time_base::dmy:
					l_strYear = l_arraySplitted[2];
					l_tmbuf.tm_mday = to_int( l_arraySplitted[0] );
					l_tmbuf.tm_mon = DoGetMonthIndex( l_arraySplitted[1] );
					l_tmbuf.tm_year = to_int( l_arraySplitted[2] );
					break;

				case std::time_base::mdy:
					l_strYear = l_arraySplitted[2];
					l_tmbuf.tm_mon = DoGetMonthIndex( l_arraySplitted[0] );
					l_tmbuf.tm_mday = to_int( l_arraySplitted[1] );
					l_tmbuf.tm_year = to_int( l_arraySplitted[2] );
					break;

				case std::time_base::ydm:
					l_strYear = l_arraySplitted[0];
					l_tmbuf.tm_year = to_int( l_arraySplitted[0] );
					l_tmbuf.tm_mday = to_int( l_arraySplitted[1] );
					l_tmbuf.tm_mon = DoGetMonthIndex( l_arraySplitted[2] );
					break;

				case std::time_base::ymd:
					l_strYear = l_arraySplitted[0];
					l_tmbuf.tm_year = to_int( l_arraySplitted[0] );
					l_tmbuf.tm_mon = DoGetMonthIndex( l_arraySplitted[1] );
					l_tmbuf.tm_mday = to_int( l_arraySplitted[2] );
					break;

				default:
					l_bReturn = false;
				}

				l_bReturn &= l_tmbuf.tm_mon > 0 && l_tmbuf.tm_mon <= 12;

				if ( l_bReturn )
				{
					if ( l_strYear.size() > 2 )
					{
						l_bReturn = l_tmbuf.tm_year > 1900 && l_tmbuf.tm_year < 2500;
					}
					else if ( l_strYear.size() == 2 )
					{
						l_bReturn = true;
					}
				}

				if ( l_bReturn )
				{
					l_bReturn = DoIsValidDay( l_tmbuf.tm_mday, l_tmbuf.tm_mon, l_tmbuf.tm_year );
				}
			}

			return l_bReturn;
		}

		bool is_upper_case( String const & p_strToTest )
		{
			return p_strToTest == upper_case( p_strToTest );
		}

		bool is_lower_case( String const & p_strToTest )
		{
			return p_strToTest == lower_case( p_strToTest );
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

			if ( ! p_str.empty() && ! p_delims.empty() && p_maxSplits > 0 )
			{
				l_arrayReturn.reserve( p_maxSplits + 1 );
				std::size_t l_numSplits = 0;
				std::size_t	l_pos		= 0;
				std::size_t	l_start		= 0;

				do
				{
					l_pos = p_str.find_first_of( p_delims, l_start );

					if ( l_pos == l_start )
					{
						l_start = l_pos + 1;

						if ( p_bKeepVoid )
						{
							l_arrayReturn.push_back( cuT( "" ) );
						}
					}
					else if ( l_pos == std::string::npos || l_numSplits == p_maxSplits )
					{
						l_arrayReturn.push_back( p_str.substr( l_start ) );
						return l_arrayReturn;
					}
					else
					{
						l_arrayReturn.push_back( p_str.substr( l_start, l_pos - l_start ) );
						l_start = l_pos + 1;
					}

					l_start = p_str.find_first_not_of( p_delims, l_start );
					++ l_numSplits;
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

		String from_str( std::string const & p_strString )
		{
			String l_strReturn;
			std::wostringstream l_wstream;
			str_converter< char, xchar >()( p_strString, l_strReturn, l_wstream.getloc() );
			return l_strReturn;
		}

		String from_wstr( std::wstring const & p_strString )
		{
			String l_strReturn;
			std::wostringstream l_wstream;
			str_converter< wchar_t, xchar >()( p_strString, l_strReturn, l_wstream.getloc() );
			return l_strReturn;
		}

		std::string to_str( std::wstring const & p_str )
		{
			std::string l_strReturn;
			std::wostringstream l_wstream;
			str_converter< wchar_t, char >()( p_str, l_strReturn, l_wstream.getloc() );
			return l_strReturn;
		}

		std::wstring to_wstr( std::string const & p_str )
		{
			std::wstring l_strReturn;
			std::wostringstream l_wstream;
			str_converter< char, wchar_t >()( p_str, l_strReturn, l_wstream.getloc() );
			return l_strReturn;
		}

		String from_char( char p_char )
		{
			char l_szTmp[2] = { p_char, '\0' };
			return from_str( l_szTmp );
		}

		String from_wchar( wchar_t p_wchar )
		{
			wchar_t l_wszTmp[2] = { p_wchar, L'\0' };
			return from_wstr( l_wszTmp );
		}
	}
}
