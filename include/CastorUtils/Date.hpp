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
#ifndef ___Date___
#define ___Date___

#include "CastorUtils.hpp"

#include <ctime>
#include <locale>

namespace Castor
{	namespace Utils
{
	class DateTime;

	//! Date helper class
	class Date
	{
	protected:
		friend class DateTime;

		std::tm m_date;
		std::locale m_locale;

		static int MonthMaxDays[12];
		static std::map <String, int> AbbrDays;
		static std::map <String, int> FullDays;
		static std::map <String, int> AbbrMonths;
		static std::map <String, int> FullMonths;

	public:
		Date();
		Date( std::locale const & p_locale);
		Date( Date const & p_dtDate);
		Date( int p_iYear, eMONTH p_eMonth, int p_iDay);
		Date( std::tm const & p_pTm);
		Date( DateTime const & p_dtDateTime);
		Date( String const & p_strDate, xchar const * p_pFormat = NULL);

		bool	Parse		( String const & p_strDate, xchar const * p_pFormat = NULL);
		bool	Parse		( String const & p_strDate, xchar const * p_pFormat, std::locale const & p_locale);
		String	Format		( xchar const * p_pFormat);
		String	Format		( xchar const * p_pFormat, std::locale const & p_locale);
		void	ToStdTm		( std::tm & p_pTm)const;
		void	FromStdTm	( std::tm const & p_pTm);

		inline int						GetYear		()const { return m_date.tm_year + 1900; }
		inline eMONTH					GetMonth	()const { return eMONTH( m_date.tm_mon + 1); }
		inline int						GetMonthDay	()const { return m_date.tm_mday; }
		inline int						GetYearDay	()const { return m_date.tm_yday + 1; }
		inline eWEEK_DAY				GetWeekDay	()const { return eWEEK_DAY( m_date.tm_wday + 1); }
		inline const std::locale	&	GetLocale	()const { return m_locale; }

		inline void SetLocale	( std::locale const & val)	{ m_locale = val; }

		static bool IsDate( String const & p_strDate, xchar const * p_pFormat);
		static bool IsDate( String const & p_strDate, xchar const * p_pFormat, Date & p_dtResult);

		bool IsValid()const;

		Date &	operator = ( Date const & p_dtDate);
		bool	operator ==( Date const & p_dtDate)const;
		bool	operator !=( Date const & p_dtDate)const;
		bool	operator <=( Date const & p_dtDate)const;
		bool	operator >=( Date const & p_dtDate)const;
		bool	operator < ( Date const & p_dtDate)const;
		bool	operator > ( Date const & p_dtDate)const;

	private:
		bool _parse				( String const & p_strDate, xchar const * p_pFormat, std::locale const & p_locale);
		bool _findName			( String & p_strDateLeft, std::map<String, int> const & p_map, int & p_iResult);
		bool _guessxFormat		( String & p_strResult);
		void _checkValidity		();
		void _computeWeekDay	();
		void _computeYearDay	();
	};
}
}

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, Castor::Utils::Date const & p_dtDate);
std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, Castor::Utils::Date & p_dtDate);

#endif
