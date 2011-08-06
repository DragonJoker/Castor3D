/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_DateTime___
#define ___CU_DateTime___

#include "CastorUtils.hpp"

#include <ctime>
#include <locale>

namespace Castor
{	namespace Utils
{
	class Time;
	class Date;

	//! Date and Time helper class
	class DateTime
	{
	protected:
		friend class Time;
		friend class Date;

		std::tm m_dateTime;
		std::locale m_locale;

		static std::map <String, int> AbbrDays;
		static std::map <String, int> FullDays;
		static std::map <String, int> AbbrMonths;
		static std::map <String, int> FullMonths;

	public:
		DateTime();
		DateTime( std::locale const & p_locale);
		DateTime( DateTime const & p_dtDateTime);
		DateTime( int p_iYear, eMONTH p_eMonth, int p_iDay, int p_iHour, int p_iMinute, int p_iSecond);
		DateTime( std::tm const & p_pTm);
		DateTime( Date const & p_dtDate);
		DateTime( Time const & p_dtTime);
		DateTime( String const & p_strTime, xchar const * p_pFormat = NULL);

		bool	Parse		( String const & p_strDateTime, xchar const * p_pFormat = NULL);
		bool	Parse		( String const & p_strDateTime, xchar const * p_pFormat, std::locale const & p_locale);
		String	Format		( xchar const * p_pFormat);
		String	Format		( xchar const * p_pFormat, std::locale const & p_locale);
		void	ToStdTm		( std::tm & p_pTm)const;
		void	FromStdTm	( std::tm const & p_pTm);

		inline int					GetYear		()const { return m_dateTime.tm_year + 1900; }
		inline eMONTH				GetMonth	()const { return eMONTH( m_dateTime.tm_mon + 1); }
		inline int					GetMonthDay	()const { return m_dateTime.tm_mday; }
		inline int					GetYearDay	()const { return m_dateTime.tm_yday + 1; }
		inline eWEEK_DAY			GetWeekDay	()const { return eWEEK_DAY( m_dateTime.tm_wday + 1); }
		inline int					GetHour		()const { return m_dateTime.tm_hour; }
		inline int					GetMinute	()const { return m_dateTime.tm_min; }
		inline int					GetSecond	()const { return m_dateTime.tm_sec; }
		inline std::locale const &	GetLocale	()const { return m_locale; }

		inline void SetLocale	( std::locale const & val)	{ m_locale = val; }

		bool IsValid()const;

		DateTime &	operator = ( DateTime const & dtTime);
		bool		operator ==( DateTime const & dtDate)const;
		bool		operator !=( DateTime const & dtDate)const;
		bool		operator <=( DateTime const & dtDate)const;
		bool		operator >=( DateTime const & dtDate)const;
		bool		operator < ( DateTime const & dtDate)const;
		bool		operator > ( DateTime const & dtDate)const;

	private:
		bool _parse				( String const & p_strDateTime, xchar const * p_pFormat, std::locale const & p_locale);
		bool _findName			( String & p_strDateTimeLeft, std::map<String, int> const & p_map, int & p_iResult);
		bool _guessxFormat		( String & p_strResult);
		bool _guessXFormat		( String & p_strResult);
		bool _guesscFormat		( String & p_strResult);
		void _checkValidity		();
		void _computeWeekDay	();
		void _computeYearDay	();
	};
}
}

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, const Castor::Utils::DateTime & p_dtDateTime);
std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, Castor::Utils::DateTime & p_dtDateTime);

#endif
