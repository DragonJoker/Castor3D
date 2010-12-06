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

#ifndef d_dll
#define d_dll
#endif

namespace Castor
{
	class String;
	typedef std::vector <String> StringArray;

#ifndef _UNICODE
	typedef char Char;
	typedef wchar_t UnChar;
#else
	typedef wchar_t Char;
	typedef char UnChar;
#endif

	typedef std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char> > StringStream;

	class d_dll String : public std::basic_string<Char, std::char_traits<Char>, std::allocator<Char> >
	{
	private:
		typedef std::basic_string<Char, std::char_traits<Char>, std::allocator<Char> > my_type;
		typedef std::basic_string<UnChar, std::char_traits<UnChar>, std::allocator<UnChar> > my_untype;

		mutable my_untype m_strOpposite;

	public:
		String();
		String( const char * p_pChars);
		String( const wchar_t * p_pChars);
		String( char p_char);
		String( wchar_t p_char);
		String( const std::string & p_pChars);
		String( const std::wstring & p_pChars);

		const Char * c_str()const;
		const char * char_str()const;
		const wchar_t * wchar_str()const;

		String substr( size_t p_uiOff = 0, size_t p_uiCount = npos)const;

		size_t find( const std::wstring & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const std::wstring & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_of( const std::wstring & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_first_not_of( const std::wstring & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_not_of( const std::wstring & p_strToFind, size_t p_uiOffset=npos)const;

		size_t find( wchar_t p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( wchar_t p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_of( wchar_t p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_first_not_of( wchar_t p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_not_of( wchar_t p_strToFind, size_t p_uiOffset=npos)const;

		bool operator ==( const wchar_t * p_pToCompare)const;
		bool operator !=( const wchar_t * p_pToCompare)const;

		size_t find( const std::string & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( const std::string & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_of( const std::string & p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_first_not_of( const std::string & p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_not_of( const std::string & p_strToFind, size_t p_uiOffset=npos)const;

		size_t find( char p_strToFind, size_t p_uiOffset=0)const;
		size_t find_first_of( char p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_of( char p_strToFind, size_t p_uiOffset=npos)const;
		size_t find_first_not_of( char p_strToFind, size_t p_uiOffset=0)const;
		size_t find_last_not_of( char p_strToFind, size_t p_uiOffset=npos)const;

		bool operator ==( const char * p_pToCompare)const;
		bool operator !=( const char * p_pToCompare)const;

		StringArray Split( const String & p_delims, unsigned int p_maxSplits=10)const;
		String & Trim( bool p_bLeft=true, bool p_bRight=true);

		template <typename T>
		inline String & operator <<( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			*this += l_out.str();

			return *this;
		}

		template <typename T>
		inline void Parse( const T & p_val)
		{
			StringStream l_out;
			l_out << p_val;
			assign( l_out.str());
		}

		void Replace( const String & p_find, const String & p_replaced);

		bool IsInteger()const;
		bool IsFloating()const;
		short ToShort()const;
		int ToInt()const;
		long ToLong()const;
		long long ToLongLong()const;
		float ToFloat()const;
		double ToDouble()const;

		String operator + ( const char * p_pText)const;
		String operator + ( const wchar_t * p_pText)const;
		String operator + ( const String & p_strText)const;
		String operator + ( const std::string & p_strText)const;
		String operator + ( const std::wstring & p_strText)const;
		String operator +=( const char * p_pText);
		String operator +=( const wchar_t * p_pText);
		String operator +=( const String & p_strText);
		String operator +=( const std::string & p_strText);
		String operator +=( const std::wstring & p_strText);
		String operator = ( const char * p_pText);
		String operator = ( const wchar_t * p_pText);
		String operator = ( const String & p_strText);
		String operator = ( const std::string & p_strText);
		String operator = ( const std::wstring & p_strText);

	private:
#ifndef _UNICODE
		void _fromWString( const std::wstring & p_strWideString);
		void _toWString( std::wstring & p_strWideString)const;
#else
		void _fromString( const std::string & p_strString);
		void _toString( std::string & p_strString)const;
#endif
		String _add( const std::string & p_strText);
		String _add( const std::wstring & p_strText);
		void _updateOpp()const;
	};

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