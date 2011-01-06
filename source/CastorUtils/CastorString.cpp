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
	:	StringType()
{
}

String :: String( const Char * szString)
{
	clear();

	if (szString != NULL)
	{
		assign( szString);
	}
}

String :: String( const UnChar * szString)
{
	clear();

	if (szString != NULL)
	{
		m_strOpposite = StringUnType( szString);
		FromUnType( m_strOpposite);
	}

}

String :: String( const Char cChar)
{
	clear();
	Char szString[] = { cChar, 0 };
	assign( szString);
}

String :: String( const UnChar cChar)
{
	clear();
	UnChar szString[] = { cChar, 0 };
	m_strOpposite = StringUnType( szString);
	FromUnType( m_strOpposite);
}

String :: String( const StringType & strString)
	:	StringType( strString)
{
}

String :: String( const StringUnType & strString)
	:	m_strOpposite( strString)
{
	FromUnType( m_strOpposite);
}

const Char * String :: c_str()const
{
	return StringType::c_str();
}

const UnChar * String :: uc_str()const
{
	_updateOpp();
	return m_strOpposite.c_str();
}

String String :: substr( size_t p_uiOff, size_t p_uiCount)const
{
	return StringType::substr( p_uiOff, p_uiCount);
}

size_t String :: find( const StringType & p_strToFind, size_t p_uiOffset)const
{
	return StringType::find( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find( Char p_cToFind, size_t p_uiOffset)const
{
	return StringType::find( p_cToFind, p_uiOffset);
}

size_t String :: find_first_of( const StringType & p_strToFind, size_t p_uiOffset)const
{
	return StringType::find_first_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_of( Char p_cToFind, size_t p_uiOffset)const
{
	return StringType::find_first_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_of( const StringType & p_strToFind, size_t p_uiOffset)const
{
	return StringType::find_last_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_of( Char p_cToFind, size_t p_uiOffset)const
{
	return StringType::find_last_of( p_cToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const StringType & p_strToFind, size_t p_uiOffset)const
{
	return StringType::find_first_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_not_of( Char p_cToFind, size_t p_uiOffset)const
{
	return StringType::find_first_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const StringType & p_strToFind, size_t p_uiOffset)const
{
	return StringType::find_last_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_not_of( Char p_cToFind, size_t p_uiOffset)const
{
	return StringType::find_last_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find( const StringUnType & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find( UnChar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find( p_cToFind, p_uiOffset);
}

size_t String :: find_first_of( const StringUnType & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_of( UnChar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_of( const StringUnType & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_of( UnChar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_of( p_cToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const StringUnType & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_first_not_of( UnChar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_first_not_of( p_cToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const StringUnType & p_strToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_not_of( p_strToFind.c_str(), p_uiOffset);
}

size_t String :: find_last_not_of( UnChar p_cToFind, size_t p_uiOffset)const
{
	_updateOpp();
	return m_strOpposite.find_last_not_of( p_cToFind, p_uiOffset);
}

bool String :: IsDate()const
{
	bool bReturn = false;

	String strText( * this);
	strText.Replace( CU_T( "/"), CU_T( " "));
	strText.Replace( CU_T( "-"), CU_T( " "));
	while (strText.find( CU_T( "  ")) != String::npos)
	{
		strText.Replace( CU_T( "  "), CU_T( " "));
	}

	StringArray l_arrayInfos;
	l_arrayInfos = strText.Split( CU_T( " "));

	if (l_arrayInfos.size() == 3 && l_arrayInfos[0].IsInteger() && l_arrayInfos[2].IsInteger())
	{
		int iMonth = _getMonthIndex( l_arrayInfos[1]);
		bReturn = iMonth > 0 && iMonth <= 12;

		int iYear = l_arrayInfos[2].ToInt();
		if (bReturn)
		{
			if (l_arrayInfos[2].size() > 2)
			{
				bReturn = iYear > 1900 && iYear < 2500;
			}
			else if (l_arrayInfos[2].size() == 2)
			{
				bReturn = true;
			}
		}

		int iDay = l_arrayInfos[0].ToInt();
		if (bReturn)
		{
			bReturn = _isValidDay( iDay, iMonth, iYear);
		}
	}

	return bReturn;
}

bool String :: IsFloating()const
{
	bool bReturn = false;

	String strText( * this);
	strText.Replace( CU_T(","), CU_T("."));
	StringArray l_arrayParts = strText.Split( CU_T("."));
	size_t nSize = l_arrayParts.size();

	if (nSize > 0 && nSize < 3)
	{
		bReturn = l_arrayParts[0].IsInteger();

		if (bReturn && nSize > 1)
		{
			bReturn = l_arrayParts[1].IsInteger();
		}
	}

	return bReturn;
}

bool String :: IsInteger()const
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

short String :: ToShort()const
{
	short sReturn = 0;

	if ( ! empty())
	{
		sReturn = short( atoi( c_str()));
	}

	return sReturn;
}

int String :: ToInt()const
{
	int iReturn = 0;

	if ( ! empty())
	{
		iReturn = atoi( c_str());
	}

	return iReturn;
}

long String :: ToLong()const
{
	long lReturn = 0;

	if ( ! empty())
	{
		lReturn = atol( c_str());
	}

	return lReturn;
}

long long String :: ToLongLong()const
{
	long long llReturn = 0;

	if ( ! empty())
	{
		llReturn = long long( atol( c_str()));
	}

	return llReturn;
}

float String :: ToFloat()const
{
	float fReturn = 0;

	if ( ! empty())
	{
		fReturn = float( atof( c_str()));
	}

	return fReturn;
}

double String :: ToDouble()const
{
	double dReturn = 0;

	if ( ! empty())
	{
		dReturn = atof( c_str());
	}

	return dReturn;
}

real String :: ToReal()const
{
	real rReturn = 0;

	if ( ! empty())
	{
		rReturn = ator( c_str());
	}

	return rReturn;
}

bool String :: IsUpperCase()const
{
	return operator ==( UpperCase());
}

bool String :: IsLowerCase()const
{
	return operator ==( LowerCase());
}

String String :: UpperCase()const
{
	String l_strReturn;
	std::locale l_locale( "french");

	for (size_t i = 0 ; i < size() ; i++)
	{
		l_strReturn += std::toupper( at( i), l_locale);
	}

	return l_strReturn;
}

String String :: LowerCase()const
{
	String l_strReturn;
	std::locale l_locale( "french");

	for (size_t i = 0 ; i < size() ; i++)
	{
		l_strReturn += std::tolower( at( i), l_locale);
	}

	return l_strReturn;
}

void String :: ToUpperCase()
{
	clear();
	_add( UpperCase());
}

void String :: ToLowerCase()
{
	clear();
	_add( LowerCase());
}

bool String :: operator ==( Char p_cToCompare)const
{
	String l_strToCompare( p_cToCompare);
	return std::operator ==( * this, l_strToCompare);
}

bool String :: operator ==( UnChar p_cToCompare)const
{
	String l_strToCompare( p_cToCompare);
	return std::operator ==( *this, l_strToCompare);
}

bool String :: operator ==( const Char * p_pToCompare)const
{
	String l_strToCompare( p_pToCompare);
	return std::operator ==( * this, l_strToCompare);
}

bool String :: operator ==( const UnChar * p_pToCompare)const
{
	String l_strToCompare( p_pToCompare);
	return std::operator ==( *this, l_strToCompare);
}

bool String :: operator ==( const StringType & p_strToCompare)const
{
	String l_strToCompare( p_strToCompare);
	return std::operator ==( * this, l_strToCompare);
}

bool String :: operator ==( const StringUnType & p_strToCompare)const
{
	String l_strToCompare( p_strToCompare);
	return std::operator ==( * this, l_strToCompare);
}

bool String :: operator !=( Char p_cToCompare)const
{
	return ! operator ==( p_cToCompare);
}

bool String :: operator !=( UnChar p_cToCompare)const
{
	return ! operator ==( p_cToCompare);
}

bool String :: operator !=( const Char * p_pToCompare)const
{
	return ! operator ==( p_pToCompare);
}

bool String :: operator !=( const UnChar * p_pToCompare)const
{
	return ! operator ==( p_pToCompare);
}

bool String :: operator !=( const StringType & p_strToCompare)const
{
	return ! operator ==( p_strToCompare);
}

bool String :: operator !=( const StringUnType & p_strToCompare)const
{
	return ! operator ==( p_strToCompare);
}

String & String :: operator +=( Char p_cChar)
{
	return _add( String( p_cChar));
}

String & String :: operator +=( UnChar p_cChar)
{
	return _add( String( p_cChar));
}

String & String :: operator +=( const Char * p_pText)
{
	return _add( p_pText);
}

String & String :: operator +=( const UnChar * p_pText)
{
	return _add( p_pText);
}

String & String :: operator +=( const StringType & p_strText)
{
	return _add( p_strText);
}

String & String :: operator +=( const StringUnType & p_strText)
{
	return _add( p_strText);
}

String String :: operator + ( Char p_cChar)const
{
	String l_strReturn( * this);
	l_strReturn += String( p_cChar);
	return l_strReturn;
}

String String :: operator + ( UnChar p_cChar)const
{
	String l_strReturn( * this);
	l_strReturn += String( p_cChar);
	return l_strReturn;
}

String String :: operator + ( const Char * p_pText)const
{
	String l_strReturn( * this);
	l_strReturn += p_pText;
	return l_strReturn;
}

String String :: operator + ( const UnChar * p_pText)const
{
	String l_strReturn( * this);
	l_strReturn += p_pText;
	return l_strReturn;
}

String String :: operator + ( const StringType & p_strText)const
{
	String l_strReturn( * this);
	l_strReturn += p_strText;
	return l_strReturn;
}

String String :: operator + ( const StringUnType & p_strText)const
{
	String l_strReturn( * this);
	l_strReturn += p_strText;
	return l_strReturn;
}

String & String :: operator = ( Char p_cChar)
{
	clear();
	_add( String( p_cChar));
	return * this;
}

String & String :: operator = ( UnChar p_cChar)
{
	clear();
	_add( String( p_cChar));
	return * this;
}

String & String :: operator = ( const Char * p_pText)
{
	clear();
	_add( p_pText);
	return * this;
}

String & String :: operator = ( const UnChar * p_pText)
{
	clear();
	_add( p_pText);
	return * this;
}

String & String :: operator = ( const StringType & p_strText)
{
	clear();
	_add( p_strText);
	return * this;
}

String & String :: operator = ( const StringUnType & p_strText)
{
	clear();
	_add( p_strText);
	return * this;
}

StringArray String :: Split( const String & p_delims, unsigned int p_maxSplits, bool p_bKeepVoid)const
{
	StringArray l_toReturn;

	if ( ! empty() && ! p_delims.empty())
	{
		l_toReturn.reserve( p_maxSplits ? p_maxSplits + 1 : 10);

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
					l_toReturn.push_back( CU_T( ""));
				}
			}
			else if (l_pos == std::string::npos || (p_maxSplits && l_numSplits == p_maxSplits))
			{
				l_toReturn.push_back( substr( l_start));
				return l_toReturn;
			}
			else
			{
				l_toReturn.push_back( substr( l_start, l_pos - l_start));
				l_start = l_pos + 1;
			}

			l_start = find_first_not_of( p_delims, l_start);
			++ l_numSplits;
		}
		while (l_pos != std::string::npos);
	}

	return l_toReturn;
}

String & String :: Trim( bool p_bLeft, bool p_bRight)
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

void String :: Replace( const String & p_find, const String & p_replaced)
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

#ifndef _UNICODE

const char * String :: char_str()const
{
	return c_str();
}

const wchar_t * String :: wchar_str()const
{
	return uc_str();
}

void String :: FromUnType( const StringUnType & p_strString)
{
	resize( p_strString.size());
    std::locale loc( "french");
#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Narrow_s( & p_strString[0], & p_strString[p_strString.size()], '?', & operator[]( 0), size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).narrow( & p_strString[0], & p_strString[p_strString.size()], '?', & operator[]( 0));
#endif
}

void String :: ToUnType( StringUnType & p_strString)const
{
	p_strString.clear();
	p_strString.resize( size());
    std::locale loc( "french");

#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Widen_s( & operator[]( 0), & operator[]( size()), & p_strString[0], p_strString.size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).widen( & operator[]( 0), & operator[]( size()), & p_strString[0]);
#endif
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

void String :: FromUnType( const StringUnType & p_strSring)
{
	resize( p_strSring.size());
    std::locale loc( "french");
#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Widen_s( & p_strSring[0], & p_strSring[p_strSring.size()], & operator[]( 0), size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).widen( & p_strSring[0], & p_strSring[p_strSring.size()], & operator[]( 0));
#endif
}

void String :: ToUnType( StringUnType & p_strString)const
{
	p_strString.clear();
	p_strString.resize( size());
    std::locale loc( "french");
#ifdef _WIN32
	std::use_facet <std::ctype <wchar_t> > ( loc)._Narrow_s( & operator[]( 0), & operator[]( size()), '?', & p_strString[0], p_strString.size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).narrow( & operator[]( 0), & operator[]( size()), '?', & p_strString[0]);
#endif
}

#endif

void String :: _updateOpp()const
{
	ToUnType( m_strOpposite);
}

String & String :: _add( const StringType & p_strText)
{
	assign( std::operator +( * this, p_strText));
	return * this;
}

String & String :: _add( const StringUnType & p_strText)
{
	String l_strText = p_strText;
	assign( std::operator +( * this, l_strText));
	return * this;
}

int String :: _getMonthIndex( const String & p_strMonth)const
{
	int l_iReturn = 0;

	if (p_strMonth.IsInteger())
	{
		l_iReturn = p_strMonth.ToInt();
	}
	else
	{
		String l_strMonthLC = p_strMonth;
		l_strMonthLC.ToLowerCase();

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
