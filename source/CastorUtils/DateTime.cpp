#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Time.hpp"
#include "CastorUtils/Date.hpp"
#include "CastorUtils/DateTime.hpp"
#include "CastorUtils/Exception.hpp"

using namespace Castor;
using namespace Castor::Utils;

//*************************************************************************************************

void InitStdTm( std::tm & p_tm)
{
	memset( & p_tm, 0, sizeof( p_tm));
}

void CopyStdTm( std::tm & p_tm, std::tm const & p_tmCopy)
{
	memcpy( & p_tm, & p_tmCopy, sizeof( p_tm));
}

//*************************************************************************************************

std::map <String, int> DateTime::AbbrDays;
std::map <String, int> DateTime::FullDays;
std::map <String, int> DateTime::AbbrMonths;
std::map <String, int> DateTime::FullMonths;

DateTime :: DateTime()
{
	InitStdTm( m_dateTime);
}

DateTime :: DateTime( std::locale const & p_locale)
	:	m_locale( p_locale)
{
}

DateTime :: DateTime( DateTime const & p_dtDateTime)
	:	m_locale( p_dtDateTime.m_locale)
{
	FromStdTm( p_dtDateTime.m_dateTime);
}

DateTime :: DateTime( int p_iYear, eMONTH p_eMonth, int p_iDay, int p_iHour, int p_iMinute, int p_iSecond)
{
	InitStdTm( m_dateTime);
	m_dateTime.tm_year = p_iYear;
	m_dateTime.tm_mon = p_eMonth - 1;
	m_dateTime.tm_mday = p_iDay - 1;
	m_dateTime.tm_hour = p_iHour;
	m_dateTime.tm_min = p_iMinute;
	m_dateTime.tm_sec = p_iSecond;
	_checkValidity();
	_computeWeekDay();
	_computeYearDay();
}

DateTime :: DateTime( std::tm const & p_pTm)
{
	FromStdTm( p_pTm);
}

DateTime :: DateTime( Date const & p_dtDate)
	:	m_locale( p_dtDate.m_locale)
{
	FromStdTm( p_dtDate.m_date);
}

DateTime :: DateTime( Time const & p_dtTime)
	:	m_locale( p_dtTime.m_locale)
{
	FromStdTm( p_dtTime.m_time);
}

DateTime :: DateTime( String const & p_strDateTime, xchar const * p_pFormat)
{
	Parse( p_strDateTime, p_pFormat);
}

bool DateTime :: Parse( String const & p_strDateTime, xchar const * p_pFormat)
{
	return _parse( p_strDateTime, p_pFormat, m_locale);
}

bool DateTime :: Parse( String const & p_strDateTime, xchar const * p_pFormat, std::locale const & p_locale)
{
	return _parse( p_strDateTime, p_pFormat, p_locale);
}

String DateTime :: Format( xchar const * p_pFormat)
{
	String l_strReturn;

	if (IsValid())
	{
		xchar l_pBuffer[1024];
		std::locale l_oldLoc = std::locale::global( m_locale);
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_dateTime);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_dateTime);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

String DateTime :: Format( xchar const * p_pFormat, std::locale const & p_locale)
{
	String l_strReturn;

	if (IsValid())
	{
		xchar l_pBuffer[1024];
		std::locale l_oldLoc = std::locale::global( p_locale);
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_dateTime);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_dateTime);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

void DateTime :: ToStdTm( std::tm & p_pTm)const
{
	CopyStdTm( p_pTm, m_dateTime);
}

void DateTime :: FromStdTm( std::tm const & p_pTm)
{
	CopyStdTm( m_dateTime, p_pTm);
	_checkValidity();
	_computeWeekDay();
	_computeYearDay();
}

bool DateTime :: IsValid()const
{
	bool l_bReturn = false;
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	if (l_eMonth != eMonthUndef && l_iMonthDay != 0 && l_iYear != 0)
	{
		if (l_eMonth != eMonthFeb)
		{
			if (l_iMonthDay <= Date::MonthMaxDays[l_eMonth - 1])
			{
				l_bReturn = true;
			}
		}
		else
		{
			bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);

			if (l_iMonthDay <= (Date::MonthMaxDays[l_eMonth - 1] + bLeap))
			{
				l_bReturn = true;
			}
		}
	}

	return l_bReturn && GetHour() < 24 && GetMinute() < 60 && GetSecond() < 60;
}

DateTime & DateTime :: operator = ( DateTime const & p_dtDateTime)
{
	FromStdTm( p_dtDateTime.m_dateTime);
	return * this;
}

bool DateTime :: operator ==( DateTime const & dtDateTime)const
{
	return GetHour() == dtDateTime.GetHour() && GetMinute() == dtDateTime.GetMinute() && GetSecond() == dtDateTime.GetSecond();
}

bool DateTime :: operator !=( DateTime const & dtDateTime)const
{
	return ! operator ==( dtDateTime);
}

bool DateTime :: operator <=( DateTime const & dtDateTime)const
{
	return (GetYear() < dtDateTime.GetYear())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() < dtDateTime.GetMonth())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() < dtDateTime.GetMonthDay())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() < dtDateTime.GetHour())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() == dtDateTime.GetHour() && GetMinute() < dtDateTime.GetMinute())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() == dtDateTime.GetHour() && GetMinute() == dtDateTime.GetMinute() && GetSecond() <= dtDateTime.GetSecond());
}

bool DateTime :: operator >=( DateTime const & dtDateTime)const
{
	return (GetYear() > dtDateTime.GetYear())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() > dtDateTime.GetMonth())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() > dtDateTime.GetMonthDay())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() > dtDateTime.GetHour())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() == dtDateTime.GetHour() && GetMinute() > dtDateTime.GetMinute())
		|| (GetYear() == dtDateTime.GetYear() && GetMonth() == dtDateTime.GetMonth() && GetMonthDay() == dtDateTime.GetMonthDay() && GetHour() == dtDateTime.GetHour() && GetMinute() == dtDateTime.GetMinute() && GetSecond() >= dtDateTime.GetSecond());
}

bool DateTime :: operator < ( DateTime const & dtDateTime)const
{
	return ! operator >=( dtDateTime);
}

bool DateTime :: operator > ( DateTime const & dtDateTime)const
{
	return ! operator <=( dtDateTime);
}

bool DateTime :: _parse( String const & p_strDateTime, xchar const * p_pFormat, std::locale const & p_locale)
{
	bool l_bReturn = true;
	InitStdTm( m_dateTime);
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

	String l_strDateTimeLeft = p_strDateTime;
	String l_strFmtLeft = l_strFormat;
	String l_strWord;
	std::map <String, int>::iterator l_it;

	while (l_strFmtLeft.size() > 0 && l_strDateTimeLeft.size() > 0 && l_bReturn)
	{
		if (l_strFmtLeft[0] != l_strDateTimeLeft[0])
		{
			if (l_strFmtLeft[0] == cuT( '%'))
			{
				l_strFmtLeft = l_strFmtLeft.substr( 1);

				// Starting a format specifier
				if (l_strFmtLeft.size() > 0 && l_strDateTimeLeft.size() > 1)
				{
					xchar l_formatSpecifier = l_strFmtLeft[0];
					l_strFmtLeft = l_strFmtLeft.substr( 1);

					switch (l_formatSpecifier)
					{
					case cuT( 'a'):
						// abbreviated day name
						l_bReturn = _findName( l_strDateTimeLeft, AbbrDays, m_dateTime.tm_wday);
						break;

					case cuT( 'b'):
						// abbreviated month name
						l_bReturn = _findName( l_strDateTimeLeft, AbbrMonths, m_dateTime.tm_mon);
						break;

					case cuT( 'c'):
						// full date and time
						l_bReturn = _guesscFormat( l_strWord);
						if (l_bReturn)
						{
							l_strFmtLeft = l_strWord + l_strFmtLeft;
						}
						break;

					case cuT( 'd'):
						// 2 digits month day
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_mday = l_strWord.to_int();
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'm'):
						// 2 digits month
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_mon = l_strWord.to_int() - 1;
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'p'):
						// AM or PM
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							if (l_strWord == cuT( "PM"))
							{
								m_dateTime.tm_hour += 12;
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
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
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_year = l_strWord.to_int();
								if (m_dateTime.tm_year < 50)
								{
									m_dateTime.tm_year += 100;
								}
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'A'):
						// full day name
						l_bReturn = _findName( l_strDateTimeLeft, FullDays, m_dateTime.tm_wday);
						break;

					case cuT( 'B'):
						// full month name
						l_bReturn = _findName( l_strDateTimeLeft, FullMonths, m_dateTime.tm_mon);
						break;

					case cuT( 'H'):
						// 2 digits 24h hour
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_hour = l_strWord.to_int();
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'I'):
						// 2 digits 12h hour
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_hour += l_strWord.to_int() - 1;
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'M'):
						// 2 digits minutes
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_min = l_strWord.to_int();
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'S'):
						// 2 digits secons
						if (l_strDateTimeLeft.size() >= 2)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_sec = l_strWord.to_int();
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'X'):
						// full time
						l_bReturn = _guessXFormat( l_strWord);
						if (l_bReturn)
						{
							l_strFmtLeft = l_strWord + l_strFmtLeft;
						}
						break;

					case cuT( 'Y'):
						// 4 decimals year
						if (l_strDateTimeLeft.size() >= 4)
						{
							l_strWord = l_strDateTimeLeft.substr( 0, 4);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_dateTime.tm_year = l_strWord.to_int() - 1900;
								l_strDateTimeLeft = l_strDateTimeLeft.substr( 4);
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

			l_strDateTimeLeft = l_strDateTimeLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
		else
		{
			// Identical => nothing special to do
			l_strDateTimeLeft = l_strDateTimeLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
	}

	std::locale::global( l_oldLocale);
	return l_bReturn && IsValid();
}

bool DateTime :: _findName( String & p_strDateTimeLeft, std::map<String, int> const & p_map, int & p_iResult)
{
	bool l_bReturn = false;
	String l_strWord;
	std::map<String, int>::const_iterator l_it;

	while ( ! l_bReturn && p_strDateTimeLeft.size() > 0)
	{
		l_it = p_map.find( l_strWord.lower_case());

		if (l_it != p_map.end())
		{
			l_bReturn = true;
			p_iResult = l_it->second;
		}
		else
		{
			l_strWord += p_strDateTimeLeft[0];
			p_strDateTimeLeft = p_strDateTimeLeft.substr( 1);
		}
	}

	return l_bReturn;
}

bool DateTime :: _guessxFormat( String & p_strResult)
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

bool DateTime :: _guessXFormat( String & p_strResult)
{
	bool l_bReturn = true;

	std::tm l_tm;
	l_tm.tm_hour = 22;
	l_tm.tm_min = 50;
	l_tm.tm_sec = 32;
	char l_pBuffer[1024];
	strftime( l_pBuffer, 1024, "%X", & l_tm);
	p_strResult = l_pBuffer;
	p_strResult.replace( cuT( "PM"), cuT( "%p"));
	p_strResult.replace( cuT( "22"), cuT( "%H"));
	p_strResult.replace( cuT( "10"), cuT( "%I"));
	p_strResult.replace( cuT( "50"), cuT( "%M"));
	p_strResult.replace( cuT( "32"), cuT( "%S"));

	return l_bReturn;
}

bool DateTime :: _guesscFormat( String & p_strResult)
{
	bool l_bReturn = true;

	std::tm l_tm;
	l_tm.tm_year = 1998;
	l_tm.tm_mon = 1;
	l_tm.tm_mday = 24;
	l_tm.tm_hour = 22;
	l_tm.tm_min = 50;
	l_tm.tm_sec = 32;
	char l_pBuffer[1024];
	strftime( l_pBuffer, 1024, "%c", & l_tm);
	p_strResult = l_pBuffer;

	p_strResult.replace( cuT( "PM"), cuT( "%p"));
	p_strResult.replace( cuT( "22"), cuT( "%H"));
	p_strResult.replace( cuT( "10"), cuT( "%I"));
	p_strResult.replace( cuT( "50"), cuT( "%M"));
	p_strResult.replace( cuT( "32"), cuT( "%S"));

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

void DateTime :: _checkValidity()
{
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	if (l_eMonth != eMonthFeb)
	{
		if (l_iMonthDay > Date::MonthMaxDays[l_eMonth - 1])
		{
			CASTOR_EXCEPTION( "Mauvais nombre de jours");
		}
	}
	else
	{
		bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);
		if (l_iMonthDay > (Date::MonthMaxDays[l_eMonth - 1] + bLeap))
		{
			CASTOR_EXCEPTION( "Mauvais nombre de jours");
		}
	}
}

void DateTime :: _computeWeekDay()
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
	m_dateTime.tm_wday = eDayMon + iTotalOffset - 1;
}

void DateTime :: _computeYearDay()
{
	int l_iYear = GetYear();
	eMONTH l_eMonth = GetMonth();
	int l_iMonthDay = GetMonthDay();

	bool bLeap = (l_iYear % 4 == 0) && (l_iYear % 100 != 0 || l_iYear % 400 == 0);
	m_dateTime.tm_yday = 0;

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
			m_dateTime.tm_yday += 31;
			break;

		case eMonthFeb:
			m_dateTime.tm_yday += 28 + bLeap;
			break;

		case eMonthApr:
		case eMonthJun:
		case eMonthSep:
		case eMonthNov:
			m_dateTime.tm_yday += 30;
			break;
		}
	}

	m_dateTime.tm_yday += l_iMonthDay;
}

//*************************************************************************************************

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, DateTime const & p_dtDateTime)
{
	std::basic_ostream<xchar>::sentry l_cerberus( p_osStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		p_dtDateTime.ToStdTm( l_tmbuf);
		const std::time_put<xchar> & l_timeFacet =  std::use_facet< std::time_put<xchar> >( p_dtDateTime.GetLocale());

		if (l_timeFacet.put( p_osStream, p_osStream, p_osStream.fill(), & l_tmbuf, 'c').failed())
		{
			p_osStream.setstate( p_osStream.badbit);
		}
	}

	return p_osStream;
}

std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, DateTime & p_dtDateTime)
{
	std::basic_istream<xchar>::sentry l_cerberus( p_isStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmDate;
		std::tm l_tmTime;
		std::ios::iostate l_state;
		std::istreambuf_iterator<xchar> l_itBegin( p_isStream);
		std::istreambuf_iterator<xchar> l_itEnd;
		std::istreambuf_iterator<xchar> l_it = std::use_facet< std::time_get<xchar> >( p_dtDateTime.GetLocale()).get_date( l_itBegin, l_itEnd, p_isStream, l_state, & l_tmDate);

		if (l_state & std::ios_base::failbit)
		{
			p_isStream.setstate( p_isStream.badbit);
		}
		else
		{
			std::tm l_tmDateTime;
			CopyStdTm( l_tmDateTime, l_tmDate);
			l_it = std::use_facet< std::time_get<xchar> >( p_dtDateTime.GetLocale()).get_time( l_itBegin, l_itEnd, p_isStream, l_state, & l_tmTime);

			if (l_state & std::ios_base::failbit)
			{
				p_isStream.setstate( p_isStream.badbit);
			}
			else
			{
				l_tmDateTime.tm_hour = l_tmTime.tm_hour;
				l_tmDateTime.tm_min = l_tmTime.tm_min;
				l_tmDateTime.tm_sec = l_tmTime.tm_sec;
			}

			p_dtDateTime.FromStdTm( l_tmDateTime);
		}
	}

	return p_isStream;
}

//*************************************************************************************************
