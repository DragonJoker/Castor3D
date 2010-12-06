#include "PrecompiledHeader.h"

#include "CastorString.h"

#if CHECK_MEMORYLEAKS
#	include "Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;

String :: String()
	:	my_type()
{
}

const Char * String :: c_str()const
{
	return my_type::c_str();
}

String String :: substr( size_t p_uiOff, size_t p_uiCount)const
{
	return my_type::substr( p_uiOff, p_uiCount);
}

#ifndef _UNICODE

String :: String( const char * p_pChars)
	:	my_type()
{
	clear();

	if (p_pChars != NULL)
	{
		assign( p_pChars);
	}
}

String :: String( const wchar_t * p_pChars)
	:	my_type()
{
	clear();
	_fromWString( p_pChars);
}

String :: String( char p_char)
	:	my_type()
{
	clear();
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	assign( l_buffer);
}

String :: String( wchar_t p_char)
{
	clear();
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	_fromWString( l_buffer);
}

String :: String( const std::string & p_strString)
	:	my_type( p_strString)
{
}

String :: String( const std::wstring & p_strString)
	:	my_type()
{
	clear();
	_fromWString( p_strString);
}

const char * String :: char_str()const
{
	return my_type::c_str();
}

const wchar_t * String :: wchar_str()const
{
	_updateOpp();
	return m_strOpposite.c_str();
}

size_t String :: find( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	std::wstring l_strReturn;
	_toWString( l_strReturn);
	return l_strReturn.find( p_strToFind, p_uiOffset);
}

size_t String :: find_first_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	std::wstring l_strReturn;
	_toWString( l_strReturn);
	return l_strReturn.find_first_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	std::wstring l_strReturn;
	_toWString( l_strReturn);
	return l_strReturn.find_last_of( p_strToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	std::wstring l_strReturn;
	_toWString( l_strReturn);
	return l_strReturn.find_first_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	std::wstring l_strReturn;
	_toWString( l_strReturn);
	return l_strReturn.find_last_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return find( l_strReturn, p_uiOffset);
}

size_t String :: find_first_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_first_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_last_of( l_strReturn, p_uiOffset);
}

size_t String :: find_first_not_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_first_not_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_not_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_last_not_of( l_strReturn, p_uiOffset);
}

bool String :: operator ==( const wchar_t * p_pToCompare)const
{
	String l_strToCompare( p_pToCompare);
	return std::operator ==( *this, l_strToCompare);
}

size_t String :: find( const std::string & p_strToFind, size_t p_uiOffset)const
{
	return std::string::find( p_strToFind, p_uiOffset);
}

size_t String :: find_first_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	return std::string::find_first_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	size_t l_uiIndex = std::string::find_last_of( p_strToFind, p_uiOffset);
	return l_uiIndex;
}

size_t String :: find_first_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	return std::string::find_first_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	return std::string::find_last_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::string::find( l_strReturn, p_uiOffset);
}

size_t String :: find_first_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::string::find_first_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::string::find_last_of( l_strReturn, p_uiOffset);
}

size_t String :: find_first_not_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::string::find_first_not_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_not_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::string::find_last_not_of( l_strReturn, p_uiOffset);
}

void String :: _fromWString( const std::wstring & p_strWideString)
{
	resize( p_strWideString.size());

    std::locale loc( "french");

#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Narrow_s( & p_strWideString[0], & p_strWideString[p_strWideString.size()], '?', & operator[]( 0), size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).narrow( & p_strWideString[0], & p_strWideString[p_strWideString.size()], '?', & operator[]( 0));
#endif
}

void String :: _toWString( std::wstring & p_strWideString)const
{
	p_strWideString.clear();
	p_strWideString.resize( size());

    std::locale loc( "french");

#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Widen_s( & operator[]( 0), & operator[]( size()), & p_strWideString[0], p_strWideString.size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).widen( & operator[]( 0), & operator[]( size()), & p_strWideString[0]);
#endif
}

String String :: _add( const std::string & p_strText)
{
	assign( std::operator +( * this, p_strText));
	return * this;
}

String String :: _add( const std::wstring & p_strText)
{
	String l_strText = p_strText;
	assign( std::operator +( * this, l_strText));
	return * this;
}

void String :: _updateOpp()const
{
	_toWString( m_strOpposite);
}

#else

String :: String( const char * p_pChars)
	:	my_type()
{
	clear();
	_fromString( p_pChars);
}

String :: String( const wchar_t * p_pChars)
	:	my_type()
{
	clear();
	if (p_pChars != NULL)
	{
		assign( p_pChars);
	}
}

String :: String( char p_char)
	:	my_type()
{
	clear();
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	_fromString( l_buffer);
}

String :: String( wchar_t p_char)
	:	std::wstring()
{
	clear();
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	assign( l_buffer);
}

String :: String( const std::string & p_strString)
	:	my_type()
{
	clear();
	_fromString( p_strString);
}

String :: String( const std::wstring & p_strString)
	:	my_type( p_strString)
{
}

const char * String :: char_str()const
{
	_updateOpp();
	return m_strOpposite.c_str();
}

const wchar_t * String :: wchar_str()const
{
	return std::wstring::c_str();
}

size_t String :: find( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	_toString( l_strReturn);
	return l_strReturn.find( p_strToFind, p_uiOffset);
}

size_t String :: find_first_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	_toString( l_strReturn);
	return l_strReturn.find_first_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	_toString( l_strReturn);
	return l_strReturn.find_last_of( p_strToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	_toString( l_strReturn);
	return l_strReturn.find_first_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	_toString( l_strReturn);
	return l_strReturn.find_last_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return find( l_strReturn, p_uiOffset);
}

size_t String :: find_first_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_first_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_last_of( l_strReturn, p_uiOffset);
}

size_t String :: find_first_not_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_first_not_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_not_of( char p_char, size_t p_uiOffset)const
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::string l_strReturn;
	l_strReturn.assign( l_buffer);
	return find_last_not_of( l_strReturn, p_uiOffset);
}

bool String :: operator ==( const char * p_pToCompare)const
{
	String l_strToCompare( p_pToCompare);
	return std::operator ==( *this, l_strToCompare);
}

size_t String :: find( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	return std::wstring::find( p_strToFind, p_uiOffset);
}

size_t String :: find_first_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	return std::wstring::find_first_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	return std::wstring::find_last_of( p_strToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	return std::wstring::find_first_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const std::wstring & p_strToFind, size_t p_uiOffset)const
{
	return std::wstring::find_last_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::wstring::find( l_strReturn, p_uiOffset);
}

size_t String :: find_first_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::wstring::find_first_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::wstring::find_last_of( l_strReturn, p_uiOffset);
}

size_t String :: find_first_not_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::wstring::find_first_not_of( l_strReturn, p_uiOffset);
}

size_t String :: find_last_not_of( wchar_t p_char, size_t p_uiOffset)const
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	std::wstring l_strReturn;
	l_strReturn.assign( l_buffer);
	return std::wstring::find_last_not_of( l_strReturn, p_uiOffset);
}

void String :: _fromString( const std::string & p_strSring)
{
	resize( p_strSring.size());

    std::locale loc( "french");

#ifdef _WIN32
    std::use_facet <std::ctype <wchar_t> > ( loc)._Widen_s( & p_strSring[0], & p_strSring[p_strSring.size()], & operator[]( 0), size());
#else
	std::use_facet <std::ctype <wchar_t> > ( loc).widen( & p_strSring[0], & p_strSring[p_strSring.size()], & operator[]( 0));
#endif
}

void String :: _toString( std::string & p_strString)const
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

String String :: _add( const std::string & p_strText)
{
	String l_strText = p_strText;
	assign( std::operator +( * this, l_strText));
	return * this;
}

String String :: _add( const std::wstring & p_strText)
{
	assign( std::operator +( * this, p_strText));
	return * this;
}

void String :: _updateOpp()const
{
	_toString( m_strOpposite);
}

#endif

String String :: operator + ( const char * p_pText)const
{
	String l_strReturn( * this);
	l_strReturn._add( p_pText);
	return l_strReturn;
}

String String :: operator + ( const wchar_t * p_pText)const
{
	String l_strReturn( * this);
	l_strReturn._add( p_pText);
	return l_strReturn;
}

String String :: operator + ( const String & p_strText)const
{
	String l_strReturn( * this);
	l_strReturn._add( p_strText);
	return l_strReturn;
}

String String :: operator + ( const std::string & p_strText)const
{
	String l_strReturn( * this);
	l_strReturn._add( p_strText);
	return l_strReturn;
}

String String :: operator + ( const std::wstring & p_strText)const
{
	String l_strReturn( * this);
	l_strReturn._add( p_strText);
	return l_strReturn;
}

String String :: operator +=( const char * p_pText)
{
	return _add( p_pText);
}

String String :: operator +=( const wchar_t * p_pText)
{
	return _add( p_pText);
}

String String :: operator +=( const String & p_strText)
{
	return _add( p_strText);
}

String String :: operator +=( const std::string & p_strText)
{
	return _add( p_strText);
}

String String :: operator +=( const std::wstring & p_strText)
{
	return _add( p_strText);
}

String String :: operator = ( const char * p_pText)
{
	clear();
	_add( p_pText);
	return * this;
}

String String :: operator = ( const wchar_t * p_pText)
{
	clear();
	_add( p_pText);
	return * this;
}

String String :: operator = ( const String & p_strText)
{
	clear();
	_add( p_strText);
	return * this;
}

String String :: operator = ( const std::string & p_strText)
{
	clear();
	_add( p_strText);
	return * this;
}

String String :: operator = ( const std::wstring & p_strText)
{
	clear();
	_add( p_strText);
	return * this;
}

bool String :: operator ==( const Char * p_pToCompare)const
{
	return std::operator ==( *this, p_pToCompare);
}

StringArray String :: Split( const String & p_delims, unsigned int p_maxSplits)const
{
	StringArray l_toReturn;

	if (empty() || p_delims.empty())
	{
		return l_toReturn;
	}

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

bool String :: IsInteger()const
{
	bool l_bReturn = true;

	if ( ! empty())
	{
		l_bReturn = (at( 0) >= '0' && at( 0) <= '9') || at( 0) == '-';

		for (size_t i = 1 ; i < size() && l_bReturn ; i++)
		{
			l_bReturn = at( i) >= '0' && at( i) <= '9';
		}
	}

	return l_bReturn;
}

bool String :: IsFloating()const
{
	bool l_bReturn = true;

	String l_strText( * this);
	l_strText.Replace( ",", ".");
	StringArray l_aParts;
	l_aParts = l_strText.Split( ".", 10);
	size_t l_uiSize = l_aParts.size();

	if (l_uiSize > 0 && l_uiSize < 3)
	{
		l_bReturn = l_aParts[0].IsInteger();

		if (l_bReturn && l_uiSize > 1)
		{
			l_bReturn = l_aParts[1].IsInteger();
		}
	}

	return l_bReturn;
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

bool String :: operator !=( const wchar_t * p_pToCompare)const
{
	return ! operator ==( p_pToCompare);
}

bool String :: operator !=( const char * p_pToCompare)const
{
	return ! operator ==( p_pToCompare);
}

String Castor :: operator +( const char * p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}

String Castor :: operator +( const wchar_t * p_pText, const String & p_strText)
{
	return String( p_pText) + p_strText;
}
