#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Date.h"

using namespace Castor;
using namespace Castor::Utils;

Date :: Date( int iYear, eMONTH eMonth, int iDay)
	:	m_time( 0)
{
	std::tm l_tmbuf;
	l_tmbuf.tm_year = iYear;
	l_tmbuf.tm_mon = eMonth;
	l_tmbuf.tm_mday = iDay;
	FromCLibTm( & l_tmbuf);
}

Date :: Date( struct std::tm * p_pTm)
	:	m_time( 0)
{
	if ( p_pTm != NULL)
	{
		FromCLibTm( p_pTm);
	}
}

void Date :: ToCLibTm( struct std::tm * p_pTm)const
{
	Localtime( p_pTm, & m_time);
}

void Date :: FromCLibTm( struct std::tm * p_pTm)
{
	m_time = mktime( p_pTm);
}

int Date :: GetYear()
{
	std::tm l_tmbuf;
	ToCLibTm( & l_tmbuf);
	return l_tmbuf.tm_year;
}

Date::eMONTH Date :: GetMonth()
{
	std::tm l_tmbuf;
	ToCLibTm( & l_tmbuf);
	return eMONTH( l_tmbuf.tm_mon);
}

int Date :: GetMonthDay()
{
	std::tm l_tmbuf;
	ToCLibTm( & l_tmbuf);
	return l_tmbuf.tm_mday;
}

int Date :: GetYearDay()
{
	std::tm l_tmbuf;
	ToCLibTm( & l_tmbuf);
	return l_tmbuf.tm_yday;
}

int Date :: GetWeekDay()
{
	std::tm l_tmbuf;
	ToCLibTm( & l_tmbuf);
	return l_tmbuf.tm_wday;
}

std::ostream & operator <<( std::ostream & p_osStream, const Date & p_dtDate)
{
	std::ostream::sentry l_cerberus( p_osStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		p_dtDate.ToCLibTm( & l_tmbuf);
		const std::time_put<xchar> & l_timeFacet =  std::use_facet< std::time_put<xchar> >( p_osStream.getloc());

		if (l_timeFacet.put( p_osStream, p_osStream, p_osStream.fill(), & l_tmbuf, 'x').failed())
		{
			p_osStream.setstate( p_osStream.badbit);
		}
	}

	return p_osStream;
}

std::istream & operator >>( std::istream & p_isStream, Date & p_dtDate)
{
	std::istream::sentry l_cerberus( p_isStream);

	if (bool( l_cerberus))
	{
		std::tm l_tmbuf;
		const std::time_get<xchar> & l_timeFacet =  std::use_facet< std::time_get<xchar> >( p_isStream.getloc());
		std::time_get<xchar>::dateorder l_order = l_timeFacet.date_order();

		switch (l_order)
		{
		case std::time_base::dmy:
			p_isStream >> l_tmbuf.tm_mday;
			p_isStream >> l_tmbuf.tm_mon;
			p_isStream >> l_tmbuf.tm_year;
			break;

		case std::time_base::mdy:
			p_isStream >> l_tmbuf.tm_mon;
			p_isStream >> l_tmbuf.tm_mday;
			p_isStream >> l_tmbuf.tm_year;
			break;

		case std::time_base::ydm:
			p_isStream >> l_tmbuf.tm_year;
			p_isStream >> l_tmbuf.tm_mday;
			p_isStream >> l_tmbuf.tm_mon;
			break;

		case std::time_base::ymd:
			p_isStream >> l_tmbuf.tm_year;
			p_isStream >> l_tmbuf.tm_mon;
			p_isStream >> l_tmbuf.tm_mday;
			break;
		}

		p_dtDate.FromCLibTm( & l_tmbuf);
	}

	return p_isStream;
}