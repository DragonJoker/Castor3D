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
#ifndef ___CU_Time___
#define ___CU_Time___

#include "CastorUtils.hpp"

#include <ctime>
#include <locale>

namespace Castor
{	namespace Utils
{
	class DateTime;

	//! Time helper class
	class Time
	{
	protected:
		friend class DateTime;

		std::tm m_time;
		std::locale m_locale;

	public:
		Time();
		Time( std::locale const & p_locale);
		Time( Time const & p_dtTime);
		Time( int p_iHour, int p_iMinute, int p_iSecond);
		Time( std::tm const & p_pTm);
		Time( DateTime const & p_dtDateTime);
		Time( String const & p_strTime, xchar const * p_pFormat = NULL);

		bool	Parse		( String const & p_strTime, xchar const * p_pFormat = NULL);
		bool	Parse		( String const & p_strDate, xchar const * p_pFormat, std::locale const & p_locale);
		String	Format		( xchar const * p_pFormat);
		String	Format		( xchar const * p_pFormat, std::locale const & p_locale);
		void	ToStdTm		( std::tm & p_pTm)const;
		void	FromStdTm	( std::tm const & p_pTm);

		inline int					GetHour		()const { return m_time.tm_hour; }
		inline int					GetMinute	()const { return m_time.tm_min; }
		inline int					GetSecond	()const { return m_time.tm_sec; }
		inline std::locale const &	GetLocale	()const { return m_locale; }

		inline void SetLocale	( std::locale const & val)	{ m_locale = val; }

		bool IsValid()const;

		Time &	operator = ( Time const & dtTime);
		bool	operator ==( Time const & dtDate)const;
		bool	operator !=( Time const & dtDate)const;
		bool	operator <=( Time const & dtDate)const;
		bool	operator >=( Time const & dtDate)const;
		bool	operator < ( Time const & dtDate)const;
		bool	operator > ( Time const & dtDate)const;

	private:
		bool _parse( String const & p_strTime, xchar const * p_pFormat, std::locale const & p_locale);
		bool _guessXFormat( String & p_strResult);
	};
}
}

std::basic_ostream<xchar> & operator <<( std::basic_ostream<xchar> & p_osStream, Castor::Utils::Time const & p_dtTime);
std::basic_istream<xchar> & operator >>( std::basic_istream<xchar> & p_isStream, Castor::Utils::Time & p_dtTime);

#endif
