#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/CastorString.h"
#include <locale>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;

//*************************************************************************************************

String :: String()
	:	xstring_type()
{
}

String :: String( const xchar p_cChar)
{
	clear();
	xchar l_szString[] = { p_cChar, 0 };
	assign( l_szString);
}

String :: String( const ychar p_cChar)
{
	clear();
	ychar l_szString[] = { p_cChar, 0 };
	m_strOpposite = ystring_type( l_szString);
	from_ytype( m_strOpposite);
}

String :: String( const xchar * p_szString)
{
	clear();

	if (p_szString != NULL)
	{
		assign( p_szString);
	}
}

String :: String( const ychar * p_szString)
{
	clear();

	if (p_szString != NULL)
	{
		m_strOpposite = ystring_type( p_szString);
		from_ytype( m_strOpposite);
	}

}

String :: String( const xstring_type & p_strString)
	:	xstring_type( p_strString)
{
}

String :: String( const ystring_type & p_strString)
	:	m_strOpposite( p_strString)
{
	from_ytype( m_strOpposite);
}

String :: String( const String & p_strString)
:	xstring_type( p_strString)
{
}

const xchar * String :: c_str()const
{
	return xstring_type::c_str();
}

const ychar * String :: uc_str()const
{
	_updateOpp();
	return m_strOpposite.c_str();
}

size_t String :: find( const xstring_type & p_strToFind, size_t p_uiOffset)const
{
	return xstring_type::find( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find( xchar p_cToFind, size_t p_uiOffset)const
{
	return xstring_type::find( p_cToFind, p_uiOffset);
}

size_t String :: find( const ystring_type & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find( ychar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find( p_cToFind, p_uiOffset);
}

size_t String :: find_first_of( const xstring_type & p_strToFind, size_t p_uiOffset)const
{
	return xstring_type::find_first_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_of( xchar p_cToFind, size_t p_uiOffset)const
{
	return xstring_type::find_first_of( p_cToFind, p_uiOffset);
}

size_t String :: find_first_of( const ystring_type & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_of( ychar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_of( const xstring_type & p_strToFind, size_t p_uiOffset)const
{
	return xstring_type::find_last_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_of( xchar p_cToFind, size_t p_uiOffset)const
{
	return xstring_type::find_last_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_of( const ystring_type & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_of( ychar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_of( p_cToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const xstring_type & p_strToFind, size_t p_uiOffset)const
{
	return xstring_type::find_first_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_not_of( xchar p_cToFind, size_t p_uiOffset)const
{
	return xstring_type::find_first_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const ystring_type & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_not_of( ychar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const xstring_type & p_strToFind, size_t p_uiOffset)const
{
	return xstring_type::find_last_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_not_of( xchar p_cToFind, size_t p_uiOffset)const
{
	return xstring_type::find_last_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const ystring_type & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_not_of( ychar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_not_of( p_cToFind, p_uiOffset);
}

String String :: substr( size_t p_uiOff, size_t p_uiCount)const
{
	return xstring_type::substr( p_uiOff, p_uiCount);
}

bool String :: is_date( const std::locale & p_locale)const
{
	bool l_bReturn = false;

	String l_strText( * this);
	l_strText.replace( CU_T( "/"), CU_T( " "));
	l_strText.replace( CU_T( "-"), CU_T( " "));

	while (l_strText.find( CU_T( "  ")) != String::npos)
	{
		l_strText.replace( CU_T( "  "), CU_T( " "));
	}

	std::tm l_tmbuf = { 0 };
	const std::time_get<xchar> & l_timeFacet =  std::use_facet< std::time_get<xchar> >( p_locale);
	std::time_get<xchar>::dateorder l_order = l_timeFacet.date_order();
	StringArray l_arraySplitted = l_strText.split( CU_T( " "), 4, false);

	if (l_arraySplitted.size() == 3 && l_arraySplitted[0].is_integer() && l_arraySplitted[2].is_integer())
	{
		l_bReturn = true;
		String l_strYear, l_strMonth, l_strDay;

		switch (l_order)
		{
		case std::time_base::dmy:
			l_strYear = l_arraySplitted[2];
			l_tmbuf.tm_mday = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mon = _getMonthIndex( l_arraySplitted[1]);
			l_tmbuf.tm_year = l_arraySplitted[2].to_int();
			break;

		case std::time_base::mdy:
			l_strYear = l_arraySplitted[2];
			l_tmbuf.tm_mon = _getMonthIndex( l_arraySplitted[0]);
			l_tmbuf.tm_mday = l_arraySplitted[1].to_int();
			l_tmbuf.tm_year = l_arraySplitted[2].to_int();
			break;

		case std::time_base::ydm:
			l_strYear = l_arraySplitted[0];
			l_tmbuf.tm_year = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mday = l_arraySplitted[1].to_int();
			l_tmbuf.tm_mon = _getMonthIndex( l_arraySplitted[2]);
			break;

		case std::time_base::ymd:
			l_strYear = l_arraySplitted[0];
			l_tmbuf.tm_year = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mon = _getMonthIndex( l_arraySplitted[1]);
			l_tmbuf.tm_mday = l_arraySplitted[2].to_int();
			break;

		default:
			l_bReturn = false;
		}

		l_bReturn &= l_tmbuf.tm_mon > 0 && l_tmbuf.tm_mon <= 12;

		if (l_bReturn)
		{
			if (l_strYear.size() > 2)
			{
				l_bReturn = l_tmbuf.tm_year > 1900 && l_tmbuf.tm_year < 2500;
			}
			else if (l_strYear.size() == 2)
			{
				l_bReturn = true;
			}
		}

		if (l_bReturn)
		{
			l_bReturn = _isValidDay( l_tmbuf.tm_mday, l_tmbuf.tm_mon, l_tmbuf.tm_year);
		}
	}

	return l_bReturn;
}

bool String :: is_floating( const std::locale & p_locale)const
{
	bool bReturn = false;

	String strText( * this);
	strText.replace( CU_T(","), CU_T("."));
	StringArray l_arrayParts = strText.split( CU_T("."));
	size_t nSize = l_arrayParts.size();

	if (nSize > 0 && nSize < 3)
	{
		bReturn = l_arrayParts[0].is_integer();

		if (bReturn && nSize > 1)
		{
			bReturn = l_arrayParts[1].is_integer();
		}
	}

	return bReturn;
}

bool String :: is_integer( const std::locale & p_locale)const
{
	bool bReturn = true;

	if ( ! empty())
	{
		bReturn = (at( 0) >= '0' && at( 0) <= '9') || at( 0) == '-';

		for (size_t i = 1 ; i < size() && bReturn ; i++)
		{
			bReturn = at( i) >= '0' && at( i) <= '9';
		}
	}

	return bReturn;
}

short String :: to_short( const std::locale & p_locale)const
{
	short sReturn = 0;

	if ( ! empty())
	{
		sReturn = short( atoi( c_str()));
	}

	return sReturn;
}

int String :: to_int( const std::locale & p_locale)const
{
	int iReturn = 0;

	if ( ! empty())
	{
		iReturn = atoi( c_str());
	}

	return iReturn;
}

long String :: to_long( const std::locale & p_locale)const
{
	long lReturn = 0;

	if ( ! empty())
	{
		lReturn = atol( c_str());
	}

	return lReturn;
}

long long String :: to_long_long( const std::locale & p_locale)const
{
	long long llReturn = 0;

	if ( ! empty())
	{
		llReturn = static_cast<long long>( atol( c_str()));
	}

	return llReturn;
}

float String :: to_float( const std::locale & p_locale)const
{
	float fReturn = 0;

	if ( ! empty())
	{
		fReturn = float( atof( c_str()));
	}

	return fReturn;
}

double String :: to_double( const std::locale & p_locale)const
{
	double dReturn = 0;

	if ( ! empty())
	{
		dReturn = atof( c_str());
	}

	return dReturn;
}

real String :: to_real( const std::locale & p_locale)const
{
	real rReturn = 0;

	if ( ! empty())
	{
		rReturn = ator( c_str());
	}

	return rReturn;
}

Utils::Date	String :: to_date( const std::locale & p_locale)const
{
	Utils::Date dtReturn;

	if (is_date( p_locale))
	{
		std::tm l_tmbuf;
		const std::time_get<xchar> & l_timeFacet =  std::use_facet< std::time_get<xchar> >( p_locale);
		std::time_get<xchar>::dateorder l_order = l_timeFacet.date_order();
		StringArray l_arraySplitted = split( CU_T( " \t,;"), 4, false);

		switch (l_order)
		{
		case std::time_base::dmy:
			l_tmbuf.tm_mday = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mon = l_arraySplitted[1].to_int();
			l_tmbuf.tm_year = l_arraySplitted[2].to_int();
			break;

		case std::time_base::mdy:
			l_tmbuf.tm_mon = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mday = l_arraySplitted[1].to_int();
			l_tmbuf.tm_year = l_arraySplitted[2].to_int();
			break;

		case std::time_base::ydm:
			l_tmbuf.tm_year = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mday = l_arraySplitted[1].to_int();
			l_tmbuf.tm_mon = l_arraySplitted[2].to_int();
			break;

		case std::time_base::ymd:
			l_tmbuf.tm_year = l_arraySplitted[0].to_int();
			l_tmbuf.tm_mon = l_arraySplitted[1].to_int();
			l_tmbuf.tm_mday = l_arraySplitted[2].to_int();
			break;
		}

		dtReturn.FromCLibTm( & l_tmbuf);
	}

	return dtReturn;
}

bool String :: is_upper_case()const
{
	return operator ==( upper_case());
}

bool String :: is_lower_case()const
{
	return operator ==( lower_case());
}

String String :: upper_case()const
{
	String l_strReturn;
	std::locale loc;

	for (size_t i = 0 ; i < size() ; i++)
	{
		l_strReturn += std::toupper( at( i), loc);
	}

	return l_strReturn;
}

String String :: lower_case()const
{
	String l_strReturn;
	std::locale loc;

	for (size_t i = 0 ; i < size() ; i++)
	{
		l_strReturn += std::tolower( at( i), loc);
	}

	return l_strReturn;
}

void String :: to_upper_case()
{
	clear();
	assign( upper_case());
}

void String :: to_lower_case()
{
	clear();
	assign( lower_case());
}

bool String :: operator ==( xchar p_cToCompare)const
{
	return std::operator ==( * this, String( p_cToCompare));
}

bool String :: operator ==( ychar p_cToCompare)const
{
	return std::operator ==( * this, String( p_cToCompare));
}

bool String :: operator ==( const xchar * p_szToCompare)const
{
	return std::operator ==( * this, String( p_szToCompare));
}

bool String :: operator ==( const ychar * p_szToCompare)const
{
	return std::operator ==( *this, String( p_szToCompare));
}

bool String :: operator ==( const xstring_type & p_strToCompare)const
{
	return std::operator ==( * this, String( p_strToCompare));
}

bool String :: operator ==( const ystring_type & p_strToCompare)const
{
	return std::operator ==( * this, String( p_strToCompare));
}

bool String :: operator ==( const String & p_strToCompare)const
{
	return std::operator ==( * this, p_strToCompare);
}

bool String :: operator !=( xchar p_cToCompare)const
{
	return ! operator ==( p_cToCompare);
}

bool String :: operator !=( ychar p_cToCompare)const
{
	return ! operator ==( p_cToCompare);
}

bool String :: operator !=( const xchar * p_szToCompare)const
{
	return ! operator ==( p_szToCompare);
}

bool String :: operator !=( const ychar * p_szToCompare)const
{
	return ! operator ==( p_szToCompare);
}

bool String :: operator !=( const xstring_type & p_strToCompare)const
{
	return ! operator ==( p_strToCompare);
}

bool String :: operator !=( const ystring_type & p_strToCompare)const
{
	return ! operator ==( p_strToCompare);
}

bool String :: operator !=( const String & p_strToCompare)const
{
	return ! operator ==( p_strToCompare);
}

String & String :: operator +=( xchar p_cChar)
{
	xstring_type::operator +=( p_cChar);
	return * this;
}

String & String :: operator +=( ychar p_cChar)
{
	xstring_type::operator +=( String( p_cChar)[0]);
	return * this;
}

String & String :: operator +=( const xchar * p_szText)
{
	xstring_type::operator +=( p_szText);
	return * this;
}

String & String :: operator +=( const ychar * p_szText)
{
	xstring_type::operator +=( String( p_szText));
	return * this;
}

String & String :: operator +=( const xstring_type & p_strText)
{
	xstring_type::operator +=( p_strText);
	return * this;
}

String & String :: operator +=( const ystring_type & p_strText)
{
	xstring_type::operator +=( String( p_strText));
	return * this;
}

String & String :: operator +=( const String & p_strText)
{
	xstring_type::operator +=( p_strText);
	return * this;
}

String String :: operator + ( xchar p_cChar)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_cChar)));
}

String String :: operator + ( ychar p_cChar)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_cChar)));
}

String String :: operator + ( const xchar * p_szText)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_szText)));
}

String String :: operator + ( const ychar * p_szText)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_szText)));
}

String String :: operator + ( const xstring_type & p_strText)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_strText)));
}

String String :: operator + ( const ystring_type & p_strText)const
{
	return std::operator +( xstring_type( * this), xstring_type( String( p_strText)));
}

String String :: operator + ( const String & p_strText)const
{
	return std::operator +( xstring_type( * this), xstring_type( p_strText));
}

String & String :: operator = ( xchar p_cChar)
{
	xstring_type::operator =( p_cChar);
	return * this;
}

String & String :: operator = ( ychar p_cChar)
{
	xstring_type::operator =( String( p_cChar)[0]);
	return * this;
}

String & String :: operator = ( const xchar * p_szText)
{
	xstring_type::operator =( p_szText);
	return * this;
}

String & String :: operator = ( const ychar * p_szText)
{
	xstring_type::operator =( String( p_szText));
	return * this;
}

String & String :: operator = ( const xstring_type & p_strText)
{
	xstring_type::operator =( p_strText);
	return * this;
}

String & String :: operator = ( const ystring_type & p_strText)
{
	xstring_type::operator =( String( p_strText));
	return * this;
}

String & String :: operator = ( const String & p_strText)
{
	xstring_type::operator =( p_strText);
	return * this;
}

StringArray String :: split( const String & p_delims, unsigned int p_maxSplits, bool p_bKeepVoid)const
{
	StringArray l_arrayReturn;

	if ( ! empty() && ! p_delims.empty() && p_maxSplits > 0)
	{
		l_arrayReturn.reserve( p_maxSplits + 1);

		size_t l_numSplits = 0;
		size_t l_start = 0;
		size_t l_pos = 0;

		do
		{
			l_pos = find_first_of( p_delims, l_start);

			if (l_pos == l_start)
			{
				l_start = l_pos + 1;

				if (p_bKeepVoid)
				{
					l_arrayReturn.push_back( CU_T( ""));
				}
			}
			else if (l_pos == std::string::npos || l_numSplits == p_maxSplits)
			{
				l_arrayReturn.push_back( substr( l_start));
				return l_arrayReturn;
			}
			else
			{
				l_arrayReturn.push_back( substr( l_start, l_pos - l_start));
				l_start = l_pos + 1;
			}

			l_start = find_first_not_of( p_delims, l_start);
			++ l_numSplits;
		}
		while (l_pos != std::string::npos);
	}

	return l_arrayReturn;
}

String & String :: trim( bool p_bLeft, bool p_bRight)
{
	if (size() > 0)
	{
		size_t l_uiIndex;

		if (p_bLeft)
		{
			l_uiIndex= find_first_not_of( ' ');

			if (l_uiIndex > 0)
			{
				if (l_uiIndex != npos)
				{
					assign( substr( l_uiIndex, String::npos));
				}
				else
				{
					clear();
				}
			}

			l_uiIndex = find_first_not_of( '\t');

			if (l_uiIndex > 0)
			{
				if (l_uiIndex != npos)
				{
					assign( substr( l_uiIndex, npos));
				}
				else
				{
					clear();
				}
			}
		}

		if (p_bRight && size() > 0)
		{
			l_uiIndex = find_last_of( ' ');

			if (l_uiIndex == size() - 1)
			{
				assign( substr( l_uiIndex, find_last_not_of( ' ')));
			}

			l_uiIndex = find_last_of( '\t');

			if (l_uiIndex == size() - 1)
			{
				assign( substr( l_uiIndex, find_last_not_of( '\t')));
			}
		}
	}

	return * this;
}

void String :: replace( const String & p_find, const String & p_replaced)
{
	String l_temp;
	String l_return;

	size_t l_currentPos = 0;

	size_t l_pos;

	while ((l_pos = find( p_find, l_currentPos)) != String::npos)
	{
		l_return.append( substr(l_currentPos, l_pos - l_currentPos));
		l_return.append( p_replaced);
		l_currentPos = l_pos + p_find.size();
	}
	if (l_currentPos != size())
	{
		l_return.append( substr(l_currentPos, l_pos - l_currentPos));
	}

	assign( l_return);
}

void String :: from_ytype( const ystring_type & p_strString)
{
	assign( convert<ychar, xchar>( p_strString));
}

void String :: to_ytype( ystring_type & p_strString)const
{
	p_strString = convert<xchar, ychar>( * this);
}

#ifndef _UNICODE

const char * String :: char_str()const
{
	return c_str();
}

const wchar_t * String :: wchar_str()const
{
	return uc_str();
}

#else

const char * String :: char_str()const
{
	return uc_str();
}

const wchar_t * String :: wchar_str()const
{
	return c_str();
}

#endif

void String :: _updateOpp()const
{
	to_ytype( m_strOpposite);
}

int String :: _getMonthIndex( const String & p_strMonth)const
{
	int l_iReturn = 0;

	if (p_strMonth.is_integer())
	{
		l_iReturn = p_strMonth.to_int();
	}
	else
	{
		String l_strMonthLC = p_strMonth;
		l_strMonthLC.to_lower_case();

		if (l_strMonthLC == CU_T( "janvier"))
		{
			l_iReturn = 1;
		}
		else if (l_strMonthLC == CU_T( "février") || l_strMonthLC == CU_T( "fevrier"))
		{
			l_iReturn = 2;
		}
		else if (l_strMonthLC == CU_T( "mars"))
		{
			l_iReturn = 3;
		}
		else if (l_strMonthLC == CU_T( "avril"))
		{
			l_iReturn = 4;
		}
		else if (l_strMonthLC == CU_T( "mai"))
		{
			l_iReturn = 5;
		}
		else if (l_strMonthLC == CU_T( "juin"))
		{
			l_iReturn = 6;
		}
		else if (l_strMonthLC == CU_T( "juillet"))
		{
			l_iReturn = 7;
		}
		else if (l_strMonthLC == CU_T( "août") || l_strMonthLC == CU_T( "aout"))
		{
			l_iReturn = 8;
		}
		else if (l_strMonthLC == CU_T( "septembre"))
		{
			l_iReturn = 9;
		}
		else if (l_strMonthLC == CU_T( "octobre"))
		{
			l_iReturn = 10;
		}
		else if (l_strMonthLC == CU_T( "novembre"))
		{
			l_iReturn = 11;
		}
		else if (l_strMonthLC == CU_T( "décembre") || l_strMonthLC == CU_T( "decembre"))
		{
			l_iReturn = 12;
		}
	}

	return l_iReturn;
}

bool String :: _isValidDay( int p_iDay, int p_iMonth, int p_iYear)const
{
	bool l_bReturn = false;

	if (p_iDay > 0)
	{
		if (p_iMonth == 1 || p_iMonth == 3 || p_iMonth == 5 || p_iMonth == 7 || p_iMonth == 8 || p_iMonth == 10 || p_iMonth == 12)
		{
			l_bReturn = p_iDay <= 31;
		}
		else if (p_iMonth != 2)
		{
			l_bReturn = p_iDay <= 30;
		}
		else if (p_iYear % 400 == 0 || (p_iYear % 4 == 0 && p_iYear % 100 != 0))
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

//*************************************************************************************************

String Castor :: operator +( char p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}

String Castor :: operator +( wchar_t p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}

String Castor :: operator +( const char * p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}

String Castor :: operator +( const wchar_t * p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}

//*************************************************************************************************
