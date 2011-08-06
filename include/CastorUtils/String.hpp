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
#ifndef ___Castor_String___
#define ___Castor_String___

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "Date.hpp"

namespace Castor
{
	class String;
	typedef std::vector <String> StringArray;		//!< String array
	typedef std::map <String, String> StrStrMap;	//!< String map ordered by string

	typedef std::allocator<xchar> xchar_allocator;
	typedef std::allocator<ychar> ychar_allocator;
	typedef std::char_traits<xchar> xchar_traits;
	typedef std::char_traits<ychar> ychar_traits;

	typedef std::basic_stringstream<xchar, xchar_traits, xchar_allocator>	StringStream;	//!< Strig stream class

	//! String class
	/*!
	Overload of std::basic_string, dependant of the character kind : Unicode or Multibyte.
	It is used to make the user not worry about the character kind : it accepts Unicode or Multibyte, can make comparisons between both, ...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class String : public std::basic_string<xchar, xchar_traits, xchar_allocator>
	{
	public:
		typedef std::basic_string<xchar, xchar_traits, xchar_allocator>	xstring_type;
		typedef std::basic_string<ychar, ychar_traits, ychar_allocator>	ystring_type;

	private:
		mutable ystring_type m_strOpposite;

	public:
		/**@name Constructors */
		//@{
		String();
		String( xchar const p_cChar);
		String( ychar const p_cChar);
		String( xchar const * p_szString);
		String( ychar const * p_szString);
		String( xstring_type const & p_strString);
		String( ystring_type const & p_strString);
		String( String const & p_strString);
		//@}

		/**@name C String accessors */
		//@{
		xchar const * c_str()const;
		ychar const * uc_str()const;
		char const * char_str()const;
		wchar_t const * wchar_str()const;
		//@}

		/**@name Overloads from basic_string */
		//@{
		inline String substr( size_t p_uiOff = 0, size_t p_uiCount = npos)const							{ return xstring_type::substr( p_uiOff, p_uiCount); }

		inline size_t find( xchar p_strToFind, size_t p_uiOffset=0)const								{ return xstring_type::find( p_strToFind, p_uiOffset); }
		inline size_t find( xstring_type const & p_strToFind, size_t p_uiOffset=0)const					{ return xstring_type::find( p_strToFind, p_uiOffset); }
		size_t find( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find( ystring_type const & p_strToFind, size_t p_uiOffset=0)const;

		inline size_t find_first_of( xchar p_strToFind, size_t p_uiOffset=0)const						{ return xstring_type::find_first_of( p_strToFind, p_uiOffset); }
		inline size_t find_first_of( xstring_type const & p_strToFind, size_t p_uiOffset=0)const		{ return xstring_type::find_first_of( p_strToFind, p_uiOffset); }
		size_t find_first_of( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( ystring_type const & p_strToFind, size_t p_uiOffset=0)const;

		inline size_t find_last_of( xchar p_strToFind, size_t p_uiOffset=npos)const						{ return xstring_type::find_last_of( p_strToFind, p_uiOffset); }
		inline size_t find_last_of( xstring_type const & p_strToFind, size_t p_uiOffset=npos)const		{ return xstring_type::find_last_of( p_strToFind, p_uiOffset); }
		size_t find_last_of( ychar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( ystring_type const & p_strToFind, size_t p_uiOffset=npos)const;

		inline size_t find_first_not_of( xchar p_strToFind, size_t p_uiOffset=0)const					{ return xstring_type::find_first_not_of( p_strToFind, p_uiOffset); }
		inline size_t find_first_not_of( xstring_type const & p_strToFind, size_t p_uiOffset=0)const	{ return xstring_type::find_first_not_of( p_strToFind, p_uiOffset); }
		size_t find_first_not_of( ychar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( ystring_type const & p_strToFind, size_t p_uiOffset=0)const;

		inline size_t find_last_not_of( xchar p_strToFind, size_t p_uiOffset=npos)const					{ return xstring_type::find_last_not_of( p_strToFind, p_uiOffset); }
		inline size_t find_last_not_of( xstring_type const & p_strToFind, size_t p_uiOffset=npos)const	{ return xstring_type::find_last_not_of( p_strToFind, p_uiOffset); }
		size_t find_last_not_of( ychar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( ystring_type const & p_strToFind, size_t p_uiOffset=npos)const;
		//@}

		/**@name Conversion functions */
		//@{
		bool 		is_integer		( std::locale const & p_locale=std::locale())const;
		bool 		is_floating		( std::locale const & p_locale=std::locale())const;
		bool 		is_date			( std::locale const & p_locale=std::locale())const;
		short 		to_short		( std::locale const & p_locale=std::locale())const;
		int 		to_int			( std::locale const & p_locale=std::locale())const;
		long 		to_long			( std::locale const & p_locale=std::locale())const;
		long long 	to_long_long	( std::locale const & p_locale=std::locale())const;
		float 		to_float		( std::locale const & p_locale=std::locale())const;
		double 		to_double		( std::locale const & p_locale=std::locale())const;
		real 		to_real			( std::locale const & p_locale=std::locale())const;
		Utils::Date	to_date			( std::locale const & p_locale=std::locale())const;

		bool 	is_upper_case	()const;
		bool 	is_lower_case	()const;
		String 	upper_case		()const;
		String 	lower_case		()const;
		void 	to_upper_case	();
		void 	to_lower_case	();
/* Doesn't work to convert from wchar_t to char
		template <typename InChar, typename OutChar>
		static std::basic_string<OutChar> convert( const std::basic_string<InChar> & p_strIn, std::locale const & loc = std::locale())
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
*/
		//@}

		/**@name Operators */
		//@{
		bool 			operator ==( xchar p_pToCompare)const;
		bool 			operator ==( xchar const * p_szToCompare)const;
		bool 			operator ==( xstring_type const & p_strToCompare)const;
		bool 			operator ==( String const & p_strToCompare)const;
		bool 			operator ==( ychar p_pToCompare)const;
		bool 			operator ==( ychar const * p_szToCompare)const;
		bool 			operator ==( ystring_type const & p_strToCompare)const;
		inline bool 	operator !=( xchar p_pToCompare)const					{ return ! operator ==( p_pToCompare); }
		inline bool 	operator !=( xchar const * p_szToCompare)const			{ return ! operator ==( p_szToCompare); }
		inline bool 	operator !=( xstring_type const & p_strToCompare)const	{ return ! operator ==( p_strToCompare); }
		inline bool 	operator !=( String const & p_strToCompare)const		{ return ! operator ==( p_strToCompare); }
		inline bool 	operator !=( ychar p_pToCompare)const					{ return ! operator ==( p_pToCompare); }
		inline bool 	operator !=( ychar const * p_szToCompare)const			{ return ! operator ==( p_szToCompare); }
		inline bool 	operator !=( ystring_type const & p_strToCompare)const	{ return ! operator ==( p_strToCompare); }
		inline String & operator +=( xchar p_pText)								{ xstring_type::operator +=( p_pText);		return * this; }
		inline String & operator +=( xchar const * p_szText)					{ xstring_type::operator +=( p_szText);		return * this; }
		inline String & operator +=( xstring_type const & p_strText)			{ xstring_type::operator +=( p_strText);	return * this; }
		inline String &	operator +=( String const & p_strText)					{ xstring_type::operator +=( p_strText);	return * this; }
		String & 		operator +=( ychar p_pText);
		String & 		operator +=( ychar const * p_szText);
		String & 		operator +=( ystring_type const & p_strText);
		inline String & operator = ( xchar p_pText)								{ xstring_type::operator =( p_pText);	return * this; }
		inline String & operator = ( xchar const * p_szText)					{ xstring_type::operator =( p_szText);	return * this; }
		inline String & operator = ( xstring_type const & p_strText)			{ xstring_type::operator =( p_strText);	return * this; }
		inline String & operator = ( String const & p_strText)					{ xstring_type::operator =( p_strText);	return * this; }
		String & 		operator = ( ychar p_pText);
		String & 		operator = ( ychar const * p_szText);
		String & 		operator = ( ystring_type const & p_strText);
		template <typename T>
		inline String & operator <<( T const & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			*this += l_out.str();

			return *this;
		}
		template <typename T>
		inline String & operator >>( T const & p_val)
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
		inline void parse( T & p_val)const
		{
			StringStream l_out( * this);
			l_out >> p_val;
		}
		void replace( String const & p_find, String const & p_replaced);
		void to_ytype( ystring_type & strText)const;
		void from_ytype( ystring_type const & strText);
		StringArray split_text( String const & p_strDelims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		StringArray split( String const & p_delims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		String & trim( bool p_bLeft=true, bool p_bRight=true);
		//@}

	private:
		void _updateOpp()const;
		int _getMonthIndex( String const & strMonth)const;
		bool _isValidDay( int iDay, int iMonth, int iYear)const;
	};

	String operator + ( String const & p_strString, String const & p_strText);

	String operator + ( String const & p_strText, char p_cChar);
	String operator + ( String const & p_strText, char const * p_pText);
	String operator + ( String const & p_strTextA, std::string const & p_strTextB);
	String operator + ( char p_cChar, String const & p_strText);
	String operator + ( char const * p_pText, String const & p_strText);
	String operator + ( std::string const & p_strTextA, String const & p_strTextB);

	String operator + ( String const & p_strText, wchar_t p_cChar);
	String operator + ( String const & p_strText, wchar_t const * p_pText);
	String operator + ( String const & p_strTextA, std::wstring const & p_strTextB);
	String operator + ( wchar_t p_cChar, String const & p_strText);
	String operator + ( wchar_t const * p_pText, String const & p_strText);
	String operator + ( std::wstring const & p_strTextA, String const & p_strTextB);

	template <typename T>
	String to_string( T const & p_tValue)
	{
		String l_strReturn;
		l_strReturn.parse( p_tValue);
		return l_strReturn;
	}

	static String cuEmptyString;
}

#endif
