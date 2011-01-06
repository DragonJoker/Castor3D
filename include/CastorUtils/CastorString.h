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

namespace Castor
{
	class String;
	typedef std::vector <String> StringArray;		//!< String array
	typedef std::map <String, String> StrStrMap;	//!< String map ordered by string

#ifndef _UNICODE
	typedef char Char;
	typedef wchar_t UnChar;
#else
	typedef wchar_t Char;
	typedef char UnChar;
#endif

	typedef std::basic_stringstream<	Char,	std::char_traits<Char>,		std::allocator<Char> >		StringStream;	//!< Strig stream class

	//! String class
	/*!
	Overload of std::basic_string, dependant of the character kind : Unicode or Multibyte.
	It is used to make the user not worry about the character kind : it accepts Unicode or Multibyte, can make comparisons with both, ...
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class String : public std::basic_string<Char, std::char_traits<Char>, std::allocator<Char> >
	{
	private:
		typedef std::basic_string<			Char,	std::char_traits<Char>,		std::allocator<Char> >		StringType;
		typedef std::basic_string<			UnChar, std::char_traits<UnChar>,	std::allocator<UnChar> >	StringUnType;
		mutable StringUnType m_strOpposite;

	public:
		/**@name Constructors */
		//@{
		String();
		String( Char p_char);
		String( UnChar p_char);
		String( const Char * p_pChars);
		String( const UnChar * p_pChars);
		String( const StringType & p_pChars);
		String( const StringUnType & p_pChars);
		//@}

		/**@name C String accessors */
		//@{
		const Char * c_str()const;
		const UnChar * uc_str()const;
		const char * char_str()const;
		const wchar_t * wchar_str()const;
		//@}

		/**@name Overloads from basic_string */
		//@{
		String substr( size_t p_uiOff = 0, size_t p_uiCount = npos)const;

		size_t find( Char p_strToFind, size_t p_uiOffset=0)const;
		size_t find( UnChar p_strToFind, size_t p_uiOffset=0)const;
		size_t find( const StringType & p_strToFind, size_t p_uiOffset=0)const;
		size_t find( const StringUnType & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_first_of( Char p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( UnChar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const StringType & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const StringUnType & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_last_of( Char p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( UnChar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( const StringType & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_of( const StringUnType & p_strToFind, size_t p_uiOffset=npos)const;

		size_t find_first_not_of( Char p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( UnChar p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( const StringType & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_not_of( const StringUnType & p_strToFind, size_t p_uiOffset=0)const;

		size_t find_last_not_of( Char p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( UnChar p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( const StringType & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_last_not_of( const StringUnType & p_strToFind, size_t p_uiOffset=npos)const;
		//@}

		/**@name Conversion functions */
		//@{
		bool 		IsInteger	()const;
		bool 		IsFloating	()const;
		bool 		IsDate		()const;
		short 		ToShort		()const;
		int 		ToInt		()const;
		long 		ToLong		()const;
		long long 	ToLongLong	()const;
		float 		ToFloat		()const;
		double 		ToDouble	()const;
		real 		ToReal		()const;

		bool 	IsUpperCase	()const;
		bool 	IsLowerCase	()const;
		String 	UpperCase	()const;
		String 	LowerCase	()const;
		void 	ToUpperCase	();
		void 	ToLowerCase	();
		//@}

		/**@name Operators */
		//@{
		bool 		operator ==( Char p_pToCompare)const;
		bool 		operator ==( UnChar p_pToCompare)const;
		bool 		operator ==( const Char * p_pToCompare)const;
		bool 		operator ==( const UnChar * p_pToCompare)const;
		bool 		operator ==( const StringType & p_pToCompare)const;
		bool 		operator ==( const StringUnType & p_pToCompare)const;
		bool 		operator !=( Char p_pToCompare)const;
		bool 		operator !=( UnChar p_pToCompare)const;
		bool 		operator !=( const Char * p_pToCompare)const;
		bool 		operator !=( const UnChar * p_pToCompare)const;
		bool 		operator !=( const StringType & p_pToCompare)const;
		bool 		operator !=( const StringUnType & p_pToCompare)const;
		String & 	operator +=( Char p_pText);
		String & 	operator +=( UnChar p_pText);
		String & 	operator +=( const Char * p_pText);
		String & 	operator +=( const UnChar * p_pText);
		String & 	operator +=( const StringType & p_strText);
		String & 	operator +=( const StringUnType & p_strText);
		String 		operator + ( Char p_pText)const;
		String 		operator + ( UnChar p_pText)const;
		String 		operator + ( const Char * p_pText)const;
		String 		operator + ( const UnChar * p_pText)const;
		String 		operator + ( const StringType & p_strText)const;
		String 		operator + ( const StringUnType & p_strText)const;
		String & 	operator = ( Char p_pText);
		String & 	operator = ( UnChar p_pText);
		String & 	operator = ( const Char * p_pText);
		String & 	operator = ( const UnChar * p_pText);
		String & 	operator = ( const StringType & p_strText);
		String & 	operator = ( const StringUnType & p_strText);
		template <typename T>
		inline String & operator <<( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			*this += l_out.str();

			return *this;
		}
		//@}

		/**@name Other functions */
		//@{
		template <typename T>
		inline void Parse( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			assign( l_out.str());
		}
		void Replace( const String & p_find, const String & p_replaced);
		void ToUnType( StringUnType & strText)const;
		void FromUnType( const StringUnType & strText);
		StringArray SplitText( const String & p_strDelims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		StringArray Split( const String & p_delims, unsigned int p_maxSplits=10, bool p_bKeepVoid=true)const;
		String & Trim( bool p_bLeft=true, bool p_bRight=true);
		//@}

	private:
		String & _add( const StringType & p_strText);
		String & _add( const StringUnType & p_strText);
		void _updateOpp()const;
		int _getMonthIndex( const String & strMonth)const;
		bool _isValidDay( int iDay, int iMonth, int iYear)const;
	};

	String operator +( char p_pText, const String & p_strText);
	String operator +( wchar_t p_pText, const String & p_strText);
	String operator +( const char * p_pText, const String & p_strText);
	String operator +( const wchar_t * p_pText, const String & p_strText);

	template <typename T>
	String ToString( const T & p_tValue)
	{
		String l_strReturn;
		l_strReturn.Parse( p_tValue);
		return l_strReturn;
	}

	static String C3DEmptyString;
}

#endif