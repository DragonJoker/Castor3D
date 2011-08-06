#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Time.hpp"
#include "CastorUtils/Date.hpp"
#include "CastorUtils/DateTime.hpp"
#include "CastorUtils/Exception.hpp"

using namespace Castor;
using namespace Castor::Utils;

//*************************************************************************************************

extern void InitStdTm( std::tm & p_tm);
extern void CopyStdTm( std::tm & p_tm, std::tm const & p_tmCopy);

//*************************************************************************************************

int Date::MonthMaxDays[12] = { 31, 28, 31, 30, 31 ,30 ,31 ,31 ,30 ,31 ,30 ,31 };
std::map <String, int> Date::AbbrDays;
std::map <String, int> Date::FullDays;
std::map <String, int> Date::AbbrMonths;
std::map <String, int> Date::FullMonths;

Date :: Date()
{
	InitStdTm( m_date);
}

Date :: Date( std::locale const & p_locale)
	:	m_locale( p_locale)
{
}

Date :: Date( Date const & p_dtDate)
	:	m_locale( p_dtDate.m_locale)
{
	FromStdTm( p_dtDate.m_date);
}

Date :: Date( int p_iYear, eMONTH p_eMonth, int p_iDay)
{
	InitStdTm( m_date);
	m_date.tm_year = p_iYear;
	m_date.tm_mon = p_eMonth - 1;
	m_date.tm_mday = p_iDay - 1;
	_checkValidity();
	_computeWeekDay();
	_computeYearDay();
}

Date :: Date( std::tm const & p_pTm)
{
	FromStdTm( p_pTm);
}

Date :: Date( DateTime const & p_dtDateTime)
	:	m_locale( p_dtDateTime.m_locale)
{
	FromStdTm( p_dtDateTime.m_dateTime);
}

Date :: Date( String const & p_strDate, xchar const * p_pFormat)
{
	Parse( p_strDate, p_pFormat);
}

bool Date :: Parse( String const & p_strDate, xchar const * p_pFormat)
{
	return _parse( p_strDate, p_pFormat, m_locale);
}

bool Date :: Parse( String const & p_strDate, xchar const * p_pFormat, std::locale const & p_locale)
{
	return _parse( p_strDate, p_pFormat, p_locale);
}

String Date :: Format( xchar const * p_pFormat)
{
	String l_strReturn;

	if (IsValid())
	{
		xchar l_pBuffer[1024];
		std::locale l_oldLoc = std::locale::global( m_locale);
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_date);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_date);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

String Date :: Format( xchar const * p_pFormat, std::locale const & p_locale)
{
	String l_strReturn;

	if (IsValid())
	{
		xchar l_pBuffer[1024];
		std::locale l_oldLoc = std::locale::global( p_locale);
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_date);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_date);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

void Date :: ToStdTm( std::tm & p_pTm)const
{
	CopyStdTm( p_pTm, m_date);
}

void Date :: FromStdTm( std::tm const & p_pTm)
{
	InitStdTm( m_date);
	m_date.tm_year = p_pTm.tm_year;
	m_date.tm_mon = p_pTm.tm_mon;
	m_date.tm_mday = p_pTm.tm_mday;
	_checkValidity();
	_computeWeekDay();
	_computeYearDay();
}

bool Date :: IsDate( String const & p_strDate, xchar const * p_pFormat)
{
	Date l_dtDate;
	return l_dtDate.Parse( p_strDate, p_pFormat);
}

bool Date :: IsDate( String const & p_strDate, xchar const * p_pFormat, Date & p_dtResult)
{
	return p_dtResult.Parse( p_strDate, p_pFormat);
}

bool Date :: IsValid()const
{
	bool l_bReturn = false;
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	if (l_eMonth != eMonthUndef && l_iMonthDay != 0 && l_iYear != 0)
	{
		if (l_eMonth != eMonthFeb)
		{
			if (l_iMonthDay <= MonthMaxDays[l_eMonth - 1])
			{
				l_bReturn = true;
			}
		}
		else
		{
			bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);

			if (l_iMonthDay <= (MonthMaxDays[l_eMonth - 1] + bLeap))
			{
				l_bReturn = true;
			}
		}
	}

	return l_bReturn;
}

Date & Date :: operator = ( Date const & p_dtDate)
{
	FromStdTm( p_dtDate.m_date);
	return * this;
}

bool Date :: operator ==( Date const & p_dtDate)const
{
	return GetYear() == p_dtDate.GetYear() && GetMonth() == p_dtDate.GetMonth() && GetMonthDay() == p_dtDate.GetMonthDay();
}

bool Date :: operator !=( Date const & p_dtDate)const
{
	return ! operator ==( p_dtDate);
}

bool Date :: operator <=( Date const & p_dtDate)const
{
	return (GetYear() < p_dtDate.GetYear())
		|| (GetYear() == p_dtDate.GetYear() && GetMonth() < p_dtDate.GetMonth())
		|| (GetYear() == p_dtDate.GetYear() && GetMonth() == p_dtDate.GetMonth() && GetMonthDay() <= p_dtDate.GetMonthDay());
}

bool Date :: operator >=( Date const & p_dtDate)const
{
	return (GetYear() > p_dtDate.GetYear())
		|| (GetYear() == p_dtDate.GetYear() && GetMonth() > p_dtDate.GetMonth())
		|| (GetYear() == p_dtDate.GetYear() && GetMonth() == p_dtDate.GetMonth() && GetMonthDay() >= p_dtDate.GetMonthDay());
}

bool Date :: operator < ( Date const & p_dtDate)const
{
	return ! operator >=( p_dtDate);
}

bool Date :: operator > ( Date const & p_dtDate)const
{
	return ! operator <=( p_dtDate);
}

bool Date :: _parse( String const & p_strDate, xchar const * p_pFormat, std::locale const & p_locale)
{
	bool l_bReturn = true;
	InitStdTm( m_date);
	String l_strFormat( p_pFormat);

	std::locale l_oldLocale = std::locale::global( p_locale);
	char l_pBuffer[1024];

	AbbrDays.clear();
	FullDays.clear();
	AbbrMonths.clear();
	FullMonths.clear();

	for (int i = 0 ; i < 7 ; i++)
	{
		std::tm l_tm = { 0, 0, 0, 0, 0, 0, i, 0, 0 };
		strftime( l_pBuffer, 1024, "%a", & l_tm);
		AbbrDays.insert( std::pair<String, int>( String( l_pBuffer).lower_case(), i));
		strftime( l_pBuffer, 1024, "%A", & l_tm);
		FullDays.insert( std::pair<String, int>( String( l_pBuffer).lower_case(), i));
	}

	for (int i = 0 ; i < 12 ; i++)
	{
		std::tm l_tm = { 0, 0, 0, 0, i, 0, 0, 0, 0 };
		strftime( l_pBuffer, 1024, "%b", & l_tm);
		AbbrMonths.insert( std::pair<String, int>( String( l_pBuffer).lower_case(), i));
		strftime( l_pBuffer, 1024, "%B", & l_tm);
		FullMonths.insert( std::pair<String, int>( String( l_pBuffer).lower_case(), i));
	}

	String l_strDateLeft = p_strDate;
	String l_strFmtLeft = l_strFormat;
	String l_strWord;
	std::map <String, int>::iterator l_it;

	while (l_strFmtLeft.size() > 0 && l_strDateLeft.size() > 0 && l_bReturn)
	{
		if (l_strFmtLeft[0] != l_strDateLeft[0])
		{
			if (l_strFmtLeft[0] == cuT( '%'))
			{
				l_strFmtLeft = l_strFmtLeft.substr( 1);

				// Starting a format specifier
				if (l_strFmtLeft.size() > 0 && l_strDateLeft.size() > 1)
				{
					xchar l_formatSpecifier = l_strFmtLeft[0];
					l_strFmtLeft = l_strFmtLeft.substr( 1);

					switch (l_formatSpecifier)
					{
					case cuT( 'a'):
						// abbreviated day name
						l_bReturn = _findName( l_strDateLeft, AbbrDays, m_date.tm_wday);
						break;

					case cuT( 'b'):
						// abbreviated month name
						l_bReturn = _findName( l_strDateLeft, AbbrMonths, m_date.tm_mon);
						break;

					case cuT( 'd'):
						// 2 digits month day
						if (l_strDateLeft.size() >= 2)
						{
							l_strWord = l_strDateLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_date.tm_mday = l_strWord.to_int();
								l_strDateLeft = l_strDateLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'm'):
						// 2 digits month
						if (l_strDateLeft.size() >= 2)
						{
							l_strWord = l_strDateLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_date.tm_mon = l_strWord.to_int() - 1;
								l_strDateLeft = l_strDateLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'x'):
						// full date
						l_bReturn = _guessxFormat( l_strWord);
						if (l_bReturn)
						{
							l_strFmtLeft = l_strWord + l_strFmtLeft;
						}
						break;

					case cuT( 'y'):
						// 2 decimals year
						if (l_strDateLeft.size() >= 2)
						{
							l_strWord = l_strDateLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_date.tm_year = l_strWord.to_int();
								if (m_date.tm_year < 50)
								{
									m_date.tm_year += 100;
								}
								l_strDateLeft = l_strDateLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'A'):
						// full day name
						l_bReturn = _findName( l_strDateLeft, FullDays, m_date.tm_wday);
						break;

					case cuT( 'B'):
						// full month name
						l_bReturn = _findName( l_strDateLeft, FullMonths, m_date.tm_mon);
						break;

					case cuT( 'Y'):
						// 4 decimals year
						if (l_strDateLeft.size() >= 4)
						{
							l_strWord = l_strDateLeft.substr( 0, 4);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_date.tm_year = l_strWord.to_int() - 1900;
								l_strDateLeft = l_strDateLeft.substr( 4);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					default:
						throw "unknown format specifier";
					}
				}
				else
				{
					// Either format string ends with a single '%' => impossible => error
					// or date string is ended => impossible => error
					l_bReturn = false;
				}
			}
			else
			{
				// The 2 strings mustn't be different except for format specifiers
				l_bReturn = false;
			}
		}
		else if (l_strFmtLeft[0] == cuT( '%'))
		{
			// we met a '%' in the date string, so it must be a '%%' in the format string
			if (l_strFmtLeft.size() > 1)
			{
				l_strFmtLeft = l_strFmtLeft.substr( 1);
			}
			else
			{
				// format string ends with a single '%' => impossible => error
				l_bReturn = false;
			}

			l_strDateLeft = l_strDateLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
		else
		{
			// Identical => nothing special to do
			l_strDateLeft = l_strDateLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
	}

	std::locale::global( l_oldLocale);
	return l_bReturn && IsValid();
}

bool Date :: _findName( String & p_strDateLeft, std::map<String, int> const & p_map, int & p_iResult)
{
	bool l_bReturn = false;
	String l_strWord;
	std::map<String, int>::const_iterator l_it;

	while ( ! l_bReturn && p_strDateLeft.size() > 0)
	{
		l_it = p_map.find( l_strWord.lower_case());

		if (l_it != p_map.end())
		{
			l_bReturn = true;
			p_iResult = l_it->second;
		}
		else
		{
			l_strWord += p_strDateLeft[0];
			p_strDateLeft = p_strDateLeft.substr( 1);
		}
	}

	return l_bReturn;
}

bool Date :: _guessxFormat( String & p_strResult)
{
	bool l_bReturn = true;

	std::tm l_tm;
	l_tm.tm_year = 1998;
	l_tm.tm_mon = 1;
	l_tm.tm_mday = 24;
	char l_pBuffer[1024];
	strftime( l_pBuffer, 1024, "%x", & l_tm);
	p_strResult = l_pBuffer;
	p_strResult.replace( cuT( "24"), cuT( "%d"));
	p_strResult.replace( cuT( "55"), cuT( "%j"));
	p_strResult.replace( cuT( "1998"), cuT( "%Y"));
	p_strResult.replace( cuT( "98"), cuT( "%y"));
	p_strResult.replace( cuT( "02"), cuT( "%m"));
	p_strResult.replace( cuT( "08"), cuT( "%W"));
	p_strResult.replace( cuT( "09"), cuT( "%U"));
	p_strResult.replace( cuT( "1"), cuT( "%w"));

	for (std::map< String, int >::iterator l_it = FullDays.begin() ; l_it != FullDays.end() ; ++l_it)
	{
		if (l_it->second == l_tm.tm_mon)
		{
			p_strResult.replace( l_it->first, cuT( "%A"));
		}
	}

	for (std::map< String, int >::iterator l_it = FullMonths.begin() ; l_it != FullMonths.end() ; ++l_it)
	{
		if (l_it->second == l_tm.tm_mon)
		{
			p_strResult.replace( l_it->first, cuT( "%B"));
		}
	}

	for (std::map< String, int >::iterator l_it = AbbrDays.begin() ; l_it != AbbrDays.end() ; ++l_it)
	{
		if (l_it->second == l_tm.tm_mon)
		{
			p_strResult.replace( l_it->first, cuT( "%a"));
		}
	}

	for (std::map< String, int >::iterator l_it = AbbrMonths.begin() ; l_it != AbbrMonths.end() ; ++l_it)
	{
		if (l_it->second == l_tm.tm_mon)
		{
			p_strResult.replace( l_it->first, cuT( "%b"));
		}
	}

	return l_bReturn;
}

void Date :: _checkValidity()
{
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	if (l_eMonth != eMonthFeb)
	{
		if (l_iMonthDay > MonthMaxDays[l_eMonth - 1])
		{
			CASTOR_EXCEPTION( "Mauvais nombre de jours");
		}
	}
	else
	{
		bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);
		if (l_iMonthDay > (MonthMaxDays[l_eMonth - 1] + bLeap))
		{
			CASTOR_EXCEPTION( "Mauvais nombre de jours");
		}
	}
}

void Date :: _computeWeekDay()
{
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	int iMonthOffset = 0;
	int iYearOffset = 0;
	int iCenturyOffset = 0;
	int iDayOffset = 0;

	int iCentury = l_iYear / 100;
	iCenturyOffset = ((39 - iCentury) % 4) * 2;

	int iYear = l_iYear - (iCentury * 100);
	iYearOffset = ((iYear / 4) + iYear) % 7;

	switch (l_eMonth)
	{
	case eMonthJan:	iMonthOffset = 0;	break;
	case eMonthFeb:	iMonthOffset = 3;	break;
	case eMonthMar:	iMonthOffset = 3;	break;
	case eMonthApr:	iMonthOffset = 6;	break;
	case eMonthMay:	iMonthOffset = 1;	break;
	case eMonthJun:	iMonthOffset = 4;	break;
	case eMonthJul:	iMonthOffset = 6;	break;
	case eMonthAug:	iMonthOffset = 2;	break;
	case eMonthSep:	iMonthOffset = 5;	break;
	case eMonthOct:	iMonthOffset = 0;	break;
	case eMonthNov:	iMonthOffset = 3;	break;
	case eMonthDec:	iMonthOffset = 5;	break;
	}

	iDayOffset = l_iMonthDay % 7;

	int iTotalOffset = ((iCenturyOffset + iYearOffset + iMonthOffset + iDayOffset) - 1) % 7;
	m_date.tm_wday = eDayMon + iTotalOffset - 1;
}

void Date :: _computeYearDay()
{
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);
	m_date.tm_yday = 0;

	for (int i = eMonthJan ; i < l_eMonth ; i++)
	{
		switch (i)
		{
		case eMonthJan:
		case eMonthMar:
		case eMonthMay:
		case eMonthJul:
		case eMonthAug:
		case eMonthOct:
			m_date.tm_yday += 31;
			break;

		case eMonthFeb:
			m_date.tm_yday += 28 + bLeap;
			break;

		case eMonthApr:
		case eMonthJun:
		case eMonthSep:
		case eMonthNov:
			m_date.tm_yday += 30;
			break;
		}
	}

	m_date.tm_yday += l_iMonthDay;
}

//*************************************************************************************************

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, Date const & p_dtDate)
{
	std::basic_ostream<xchar>::sentry l_cerberus( p_osStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		p_dtDate.ToStdTm( l_tmbuf);
		const std::time_put<xchar> & l_timeFacet =  std::use_facet< std::time_put<xchar> >( p_dtDate.GetLocale());

		if (l_timeFacet.put( p_osStream, p_osStream, p_osStream.fill(), & l_tmbuf, 'x').failed())
		{
			p_osStream.setstate( p_osStream.badbit);
		}
	}

	return p_osStream;
}

std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, Date & p_dtDate)
{
	std::basic_istream<xchar>::sentry l_cerberus( p_isStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		std::ios::iostate l_state;
		std::istreambuf_iterator<xchar> l_itBegin( p_isStream);
		std::istreambuf_iterator<xchar> l_itEnd;
		std::istreambuf_iterator<xchar> l_it = std::use_facet< std::time_get<xchar> >( p_dtDate.GetLocale()).get_date( l_itBegin, l_itEnd, p_isStream, l_state, & l_tmbuf);

		if (l_state & std::ios_base::failbit)
		{
			p_isStream.setstate( p_isStream.badbit);
		}
		else
		{
			p_dtDate.FromStdTm( l_tmbuf);
		}
	}

	return p_isStream;
}

//*************************************************************************************************
