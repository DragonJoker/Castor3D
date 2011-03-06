/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_String___
#define ___Castor_String___

#include <string>
#include <vector>
#include <sstream>
#include "Date.h"

namespace Castor
{
	class String;
	typedef std::vector <String> StringArray;		//!< String array
	typedef std::map <String, String> StrStrMap;	//!< String map ordered by string

#ifndef _UNICODE
	typedef char xchar;
	typedef wchar_t ychar;
#else
	typedef wchar_t xchar;
	typedef char ychar;
#endif

	typedef std::allocator<xchar> xchar_allocator;
	typedef std::allocator<ychar> ychar_allocator;
	typedef std::char_traits<xchar> xchar_traits;
	typedef std::char_traits<ychar> ychar_traits;

	typedef std::basic_stringstream<xchar, xchar_traits, xchar_allocator>	StringStream;	//!< Strig stream class

	//! String class
	/*!
	Overload of std::basic_string, dependant of the character kind : Unicode or Multibyte.
	It is used to make the user not worry about the character kind : it accepts Unicode or Multibyte, can make comparisons with both, ...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class String : public std::basic_string<xchar, xchar_traits, xchar_allocator>
	{
	private:
		typedef std::basic_string<xchar, xchar_traits, xchar_allocator>	xstring_type;
		typedef std::basic_string<ychar, ychar_traits, ychar_allocator>	ystring_type;
		mutable ystring_type m_strOpposite;

	public:
		/**@name Constructors */
		//@{
		String();
		String( xchar p_cChar);
		String( ychar p_cChar);
		String( const xchar * p_szString);
		String( const ychar * p_szString);
		String( const xstring_type & p_strString);
		String( const ystring_type & p_strString);
		String( const String & p_strString);
		//@}

		/**@name C String accessors */
		//@{
		const xchar * c_str()const;
		const ychar * uc_str()const;
		const char * char_str()const;
		const wchar_t * wchar_str()const;
		//@}

		/**@name Overloads from basic_string */
		//@{
		String substr( size_t p_uiOff = 0, size_t p_uiCount = npos)const;

		size_t find( xchar p_strToFind, size_t p_uiOffset=0)const;
		size_t find( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find( const xstring_type & p_strToFind, size_t p_uiOffset=0)const;
		size_t find( const ystring_type & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_first_of( xchar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const xstring_type & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const ystring_type & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_last_of( xchar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( ychar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( const xstring_type & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( const ystring_type & p_strToFind, size_t p_uiOffset=npos)const;

		size_t find_first_not_of( xchar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( const xstring_type & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( const ystring_type & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_last_not_of( xchar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( ychar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( const xstring_type & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( const ystring_type & p_strToFind, size_t p_uiOffset=npos)const;
		//@}

		/**@name Conversion functions */
		//@{
		bool 		is_integer		( const std::locale & p_locale=std::locale())const;
		bool 		is_floating		( const std::locale & p_locale=std::locale())const;
		bool 		is_date			( const std::locale & p_locale=std::locale())const;
		short 		to_short		( const std::locale & p_locale=std::locale())const;
		int 		to_int			( const std::locale & p_locale=std::locale())const;
		long 		to_long			( const std::locale & p_locale=std::locale())const;
		long long 	to_long_long	( const std::locale & p_locale=std::locale())const;
		float 		to_float		( const std::locale & p_locale=std::locale())const;
		double 		to_double		( const std::locale & p_locale=std::locale())const;
		real 		to_real			( const std::locale & p_locale=std::locale())const;
		Utils::Date	to_date			( const std::locale & p_locale=std::locale())const;

		bool 	is_upper_case	()const;
		bool 	is_lower_case	()const;
		String 	upper_case		()const;
		String 	lower_case		()const;
		void 	to_upper_case	();
		void 	to_lower_case	();

		template <typename InChar, typename OutChar>
		static std::basic_string<OutChar> convert( const std::basic_string<InChar> & p_strIn, const std::locale & loc = std::locale())
		{
			typedef typename std::codecvt <OutChar, InChar, std::mbstate_t> facet_type;
			typedef typename facet_type::result result_type;

			std::mbstate_t l_state = std::mbstate_t();
			result_type l_result;
			std::vector <OutChar> l_buffer( p_strIn.size());
			const InChar * l_pEndIn = NULL;
			OutChar * l_pEndOut = NULL;

			l_result = std::use_facet<facet_type>( loc).in(
							l_state, p_strIn.data(), p_strIn.data() + p_strIn.length(), l_pEndIn,
							& l_buffer.front(), & l_buffer.front() + l_buffer.size(), l_pEndOut
						);

			return std::basic_string<OutChar>( & l_buffer.front(), l_pEndOut);
		}
		//@}

		/**@name Operators */
		//@{
		bool 		operator ==( xchar p_pToCompare)const;
		bool 		operator ==( ychar p_pToCompare)const;
		bool 		operator ==( const xchar * p_szToCompare)const;
		bool 		operator ==( const ychar * p_szToCompare)const;
		bool 		operator ==( const xstring_type & p_strToCompare)const;
		bool 		operator ==( const ystring_type & p_strToCompare)const;
		bool 		operator ==( const String & p_strToCompare)const;
		bool 		operator !=( xchar p_pToCompare)const;
		bool 		operator !=( ychar p_pToCompare)const;
		bool 		operator !=( const xchar * p_szToCompare)const;
		bool 		operator !=( const ychar * p_szToCompare)const;
		bool 		operator !=( const xstring_type & p_strToCompare)const;
		bool 		operator !=( const ystring_type & p_strToCompare)const;
		bool 		operator !=( const String & p_strToCompare)const;
		String & 	operator +=( xchar p_pText);
		String & 	operator +=( ychar p_pText);
		String & 	operator +=( const xchar * p_szText);
		String & 	operator +=( const ychar * p_szText);
		String & 	operator +=( const xstring_type & p_strText);
		String & 	operator +=( const ystring_type & p_strText);
		String &	operator +=( const String & p_strText);
		String 		operator + ( xchar p_pText)const;
		String 		operator + ( ychar p_pText)const;
		String 		operator + ( const xchar * p_szText)const;
		String 		operator + ( const ychar * p_szText)const;
		String 		operator + ( const xstring_type & p_strText)const;
		String 		operator + ( const ystring_type & p_strText)const;
		String 		operator + ( const String & p_strText)const;
		String & 	operator = ( xchar p_pText);
		String & 	operator = ( ychar p_pText);
		String & 	operator = ( const xchar * p_szText);
		String & 	operator = ( const ychar * p_szText);
		String & 	operator = ( const xstring_type & p_strText);
		String & 	operator = ( const ystring_type & p_strText);
		String & 	operator = ( const String & p_strText);
		template <typename T>
		inline String & operator <<( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			*this += l_out.str();

			return *this;
		}
		template <typename T>
		inline String & operator >>( const T & p_val)
		{
			StringStream l_out( * this);
			l_out >> p_val;
			assign( l_out.str());
			return *this;
		}
		//@}

		/**@name Other functions */
		//@{
		template <typename T>
		inline void parse( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			assign( l_out.str());
		}
		void replace( const String & p_find, const String & p_replaced);
		void to_ytype( ystring_type & strText)const;
		void from_ytype( const ystring_type & strText);
		StringArray split_text( const String & p_strDelims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		StringArray split( const String & p_delims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		String & trim( bool p_bLeft=true, bool p_bRight=true);
		//@}

	private:
		void _updateOpp()const;
		int _getMonthIndex( const String & strMonth)const;
		bool _isValidDay( int iDay, int iMonth, int iYear)const;
	};

	String operator +( char p_pText, const String & p_strText);
	String operator +( wchar_t p_pText, const String & p_strText);
	String operator +( const char * p_pText, const String & p_strText);
	String operator +( const wchar_t * p_pText, const String & p_strText);

	template <typename T>
	String to_string( const T & p_tValue)
	{
		String l_strReturn;
		l_strReturn.parse( p_tValue);
		return l_strReturn;
	}

	static String C3DEmptyString;
}

#endif
