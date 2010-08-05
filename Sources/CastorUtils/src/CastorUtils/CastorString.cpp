#include "CastorString.h"

using namespace General::Utils;

#ifndef _UNICODE
String :: String()
	:	std::string()
{
}

String :: String( const char * p_pChars)
	:	std::string()
{
	if (p_pChars != NULL)
	{
		assign( p_pChars);
	}
}

String :: String( const wchar_t * p_pChars)
	:	std::string()
{
	_fromWString( p_pChars);
}

String :: String( char p_char)
	:	std::string()
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	assign( l_buffer);
}

String :: String( wchar_t p_char)
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	_fromWString( l_buffer);
}

String :: String( const std::string & p_strString)
	:	std::string( p_strString)
{
}

String :: String( const std::wstring & p_strString)
	:	std::string()
{
	_fromWString( p_strString);
}

const Char * String :: c_str()const
{
	return std::string::c_str();
}

const char * String :: char_str()const
{
	return std::string::c_str();
}

const wchar_t * String :: wchar_str()const
{
	_toWString( m_unstring);
	return m_unstring.c_str();
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
	resize( p_strWideString.length());

    std::locale loc( "french");

    std::use_facet <std::ctype <wchar_t> > ( loc).narrow( & p_strWideString[0], & p_strWideString[p_strWideString.length()], '?', & operator[]( 0));
}

void String :: _toWString( std::wstring & p_strWideString)const
{
	p_strWideString.clear();
	p_strWideString.resize( length());

    std::locale loc( "french");

    std::use_facet <std::ctype <wchar_t> > ( loc).widen( & operator[]( 0), & operator[]( length()), & p_strWideString[0]);
}
#else
String :: String()
	:	std::wstring()
{
}

String :: String( const char * p_pChars)
	:	std::wstring()
{
	fromString( p_pChars);
}

String :: String( const wchar_t * p_pChars)
	:	std::wstring()
{
	if (p_pChars != NULL)
	{
		assign( p_pChars);
	}
}

String :: String( char p_char)
	:	std::wstring()
{
	char l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	fromString( l_buffer);
}

String :: String( wchar_t p_char)
	:	std::wstring()
{
	wchar_t l_buffer[2];
	l_buffer[0] = p_char;
	l_buffer[1] = 0;
	assign( l_buffer);
}

String :: String( const std::string & p_strString)
	:	std::wstring()
{
	fromString( p_strString);
}

String :: String( const std::wstring & p_strString)
	:	std::wstring( p_strString)
{
}

const Char * String :: c_str()const
{
	return std::wstring::c_str();
}

const char * String :: char_str()const
{
	toString( m_unstring);
	return m_unstring.c_str();
}

const wchar_t * String :: wchar_str()const
{
	return std::wstring::c_str();
}

size_t String :: find( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	toString( l_strReturn);
	return l_strReturn.find( p_strToFind, p_uiOffset);
}

size_t String :: find_first_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	toString( l_strReturn);
	return l_strReturn.find_first_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	toString( l_strReturn);
	return l_strReturn.find_last_of( p_strToFind, p_uiOffset);
}

size_t String :: find_first_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	toString( l_strReturn);
	return l_strReturn.find_first_not_of( p_strToFind, p_uiOffset);
}

size_t String :: find_last_not_of( const std::string & p_strToFind, size_t p_uiOffset)const
{
	std::string l_strReturn;
	toString( l_strReturn);
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

void String :: fromString( const std::string & p_strSring)
{
	resize( p_strSring.length());

    std::locale loc( "french");

    std::use_facet <std::ctype <wchar_t> > ( loc).widen( & p_strSring[0], & p_strSring[p_strSring.length()], & operator[]( 0));
}

void String :: toString( std::string & p_strString)const
{
	p_strString.clear();
	p_strString.resize( length());

    std::locale loc( "french");

    std::use_facet <std::ctype <wchar_t> > ( loc).narrow( & operator[]( 0), & operator[]( length()), '?', & p_strString[0]);
}
#endif

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
