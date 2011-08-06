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

Time :: Time()
{
	InitStdTm( m_time);
}

Time :: Time( std::locale const & p_locale)
	:	m_locale( p_locale)
{
}

Time :: Time( Time const & p_dtTime)
	:	m_locale( p_dtTime.m_locale)
{
	CopyStdTm( m_time, p_dtTime.m_time);
}

Time :: Time( int p_iHour, int p_iMinute, int p_iSecond)
{
	InitStdTm( m_time);
	m_time.tm_hour = p_iHour;
	m_time.tm_min = p_iMinute;
	m_time.tm_sec = p_iSecond;
}

Time :: Time( std::tm const & p_pTm)
{
	CopyStdTm( m_time, p_pTm);
}

Time :: Time( DateTime const & p_dtDateTime)
	:	m_locale( p_dtDateTime.m_locale)
{
	InitStdTm( m_time);
	m_time.tm_hour = p_dtDateTime.m_dateTime.tm_hour;
	m_time.tm_min = p_dtDateTime.m_dateTime.tm_min;
	m_time.tm_sec = p_dtDateTime.m_dateTime.tm_sec;
}

Time :: Time( String const & p_strTime, xchar const * p_pFormat)
{
	Parse( p_strTime, p_pFormat);
}

bool Time :: Parse( String const & p_strTime, xchar const * p_pFormat)
{
	return _parse( p_strTime, p_pFormat, m_locale);
}

bool Time :: Parse( String const & p_strTime, xchar const * p_pFormat, std::locale const & p_locale)
{
	return _parse( p_strTime, p_pFormat, p_locale);
}

String Time :: Format( xchar const * p_pFormat)
{
	String l_strReturn;

	if (IsValid())
	{
		std::locale l_oldLoc = std::locale::global( m_locale);
		xchar l_pBuffer[1024];
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_time);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_time);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

String Time :: Format( xchar const * p_pFormat, std::locale const & p_locale)
{
	String l_strReturn;

	if (IsValid())
	{
		std::locale l_oldLoc = std::locale::global( p_locale);
		xchar l_pBuffer[1024];
#ifdef _UNICODE
		wcsftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_time);
#else
		strftime( l_pBuffer, sizeof( l_pBuffer), p_pFormat, & m_time);
#endif
		std::locale::global( l_oldLoc);
		l_strReturn = l_pBuffer;
	}

	return l_strReturn;
}

void Time :: ToStdTm( std::tm & p_pTm)const
{
	CopyStdTm( p_pTm, m_time);
}

void Time :: FromStdTm( std::tm const & p_pTm)
{
	InitStdTm( m_time);
	m_time.tm_hour = p_pTm.tm_hour;
	m_time.tm_min = p_pTm.tm_min;
	m_time.tm_sec = p_pTm.tm_sec;
}

bool Time :: IsValid()const
{
	return GetHour() < 24 && GetMinute() < 60 && GetSecond() < 60;
}

Time & Time :: operator = ( Time const & p_dtTime)
{
	FromStdTm( p_dtTime.m_time);
	return * this;
}

bool Time :: operator ==( Time const & dtTime)const
{
	return GetHour() == dtTime.GetHour() && GetMinute() == dtTime.GetMinute() && GetSecond() == dtTime.GetSecond();
}

bool Time :: operator !=( Time const & dtTime)const
{
	return ! operator ==( dtTime);
}

bool Time :: operator <=( Time const & dtTime)const
{
	return (GetHour() < dtTime.GetHour())
		|| (GetHour() == dtTime.GetHour() && GetMinute() < dtTime.GetMinute())
		|| (GetHour() == dtTime.GetHour() && GetMinute() == dtTime.GetMinute() && GetSecond() <= dtTime.GetSecond());
}

bool Time :: operator >=( Time const & dtTime)const
{
	return (GetHour() > dtTime.GetHour())
		|| (GetHour() == dtTime.GetHour() && GetMinute() > dtTime.GetMinute())
		|| (GetHour() == dtTime.GetHour() && GetMinute() == dtTime.GetMinute() && GetSecond() >= dtTime.GetSecond());
}

bool Time :: operator < ( Time const & dtTime)const
{
	return ! operator >=( dtTime);
}

bool Time :: operator > ( Time const & dtTime)const
{
	return ! operator <=( dtTime);
}

bool Time :: _parse( String const & p_strTime, xchar const * p_pFormat, std::locale const & p_locale)
{
	bool l_bReturn = true;
	InitStdTm( m_time);
	String l_strFormat( p_pFormat);

	std::locale l_oldLocale = std::locale::global( p_locale);

	String l_strTimeLeft = p_strTime;
	String l_strFmtLeft = l_strFormat;
	String l_strWord;
	std::map <String, int>::iterator l_it;

	while (l_strFmtLeft.size() > 0 && l_strTimeLeft.size() > 0 && l_bReturn)
	{
		if (l_strFmtLeft[0] != l_strTimeLeft[0])
		{
			if (l_strFmtLeft[0] == cuT( '%'))
			{
				l_strFmtLeft = l_strFmtLeft.substr( 1);

				// Starting a format specifier
				if (l_strFmtLeft.size() > 0 && l_strTimeLeft.size() > 1)
				{
					xchar l_formatSpecifier = l_strFmtLeft[0];
					l_strFmtLeft = l_strFmtLeft.substr( 1);

					switch (l_formatSpecifier)
					{
					case cuT( 'p'):
						// AM or PM
						if (l_strTimeLeft.size() >= 2)
						{
							l_strWord = l_strTimeLeft.substr( 0, 2);
							if (l_strWord == cuT( "PM"))
							{
								m_time.tm_hour += 12;
								l_strTimeLeft = l_strTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'H'):
						// 2 digits 24h hour
						if (l_strTimeLeft.size() >= 2)
						{
							l_strWord = l_strTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_time.tm_hour = l_strWord.to_int();
								l_strTimeLeft = l_strTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'I'):
						// 2 digits 12h hour
						if (l_strTimeLeft.size() >= 2)
						{
							l_strWord = l_strTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_time.tm_hour += l_strWord.to_int() - 1;
								l_strTimeLeft = l_strTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'M'):
						// 2 digits minutes
						if (l_strTimeLeft.size() >= 2)
						{
							l_strWord = l_strTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_time.tm_min = l_strWord.to_int();
								l_strTimeLeft = l_strTimeLeft.substr( 2);
							}
						}
						else
						{
							l_bReturn = false;
						}
						break;

					case cuT( 'S'):
						// 2 digits secons
						if (l_strTimeLeft.size() >= 2)
						{
							l_strWord = l_strTimeLeft.substr( 0, 2);
							l_bReturn = l_strWord.is_integer();
							if (l_bReturn)
							{
								m_time.tm_sec = l_strWord.to_int();
								l_strTimeLeft = l_strTimeLeft.substr( 2);
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

			l_strTimeLeft = l_strTimeLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
		else
		{
			// Identical => nothing special to do
			l_strTimeLeft = l_strTimeLeft.substr( 1);
			l_strFmtLeft = l_strFmtLeft.substr( 1);
		}
	}

	std::locale::global( l_oldLocale);
	return l_bReturn && IsValid();

	return l_bReturn;
}

bool Time :: _guessXFormat( String & p_strResult)
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

//*************************************************************************************************

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, Time const & p_dtTime)
{
	std::basic_ostream<xchar>::sentry l_cerberus( p_osStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		p_dtTime.ToStdTm( l_tmbuf);
		const std::time_put<xchar> & l_timeFacet =  std::use_facet< std::time_put<xchar> >( p_dtTime.GetLocale());

		if (l_timeFacet.put( p_osStream, p_osStream, p_osStream.fill(), & l_tmbuf, 'X').failed())
		{
			p_osStream.setstate( p_osStream.badbit);
		}
	}

	return p_osStream;
}

std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, Time & p_dtTime)
{
	std::basic_istream<xchar>::sentry l_cerberus( p_isStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		std::ios::iostate l_state;
		std::istreambuf_iterator<xchar> l_itBegin( p_isStream);
		std::istreambuf_iterator<xchar> l_itEnd;
		std::istreambuf_iterator<xchar> l_it =  std::use_facet< std::time_get<xchar> >( p_dtTime.GetLocale()).get_time( l_itBegin, l_itEnd, p_isStream, l_state, & l_tmbuf);

		if (l_state & std::ios_base::failbit)
		{
			p_isStream.setstate( p_isStream.badbit);
		}
		else
		{
			p_dtTime.FromStdTm( l_tmbuf);
		}
	}

	return p_isStream;
}

//*************************************************************************************************
