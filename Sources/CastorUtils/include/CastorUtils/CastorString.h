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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_String___
#define ___CU_String___

#include <string>
#include <vector>

#ifndef d_dll
#define d_dll
#endif

namespace General
{	namespace Utils
{
	class String;
	typedef std::vector <String> StringArray;

#ifndef _UNICODE
	typedef char Char;
	typedef std::stringstream StringStream;

	class d_dll String : public std::string
	{
	public:
		typedef std::string MyString;
		typedef std::wstring MyUnString;
#else
	typedef wchar_t Char;
	typedef std::wstringstream StringStream;

	class d_dll String : public std::wstring
	{
	public:
		typedef std::wstring MyString;
		typedef std::string MyUnString;
#endif
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

		String substr( size_t p_begin, size_t p_count = String::npos )const
		{
			return MyString::substr( p_begin, p_count );
		}

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

		inline String & operator+=( const String & p_rhs )
		{
			MyString::operator+=( p_rhs );
			return *this;
		}

		inline String & operator+=( const std::string & p_rhs )
		{
			MyString::operator+=( String( p_rhs));
			return *this;
		}

		inline String & operator+=( const std::wstring & p_rhs )
		{
			MyString::operator+=( String( p_rhs));
			return *this;
		}

		inline String & operator+=( const char * p_rhs )
		{
			MyString::operator+=( String( p_rhs));
			return *this;
		}

		inline String & operator+=( const wchar_t * p_rhs )
		{
			MyString::operator+=( String( p_rhs));
			return *this;
		}

#ifndef _UNICODE
		void fromWString( const std::wstring & p_strWideString);
		void toWString( std::wstring & p_strWideString)const;
#else
		void fromString( const std::string & p_strString);
		void toString( std::string & p_strString)const;
#endif

	private:
		mutable MyUnString m_unstring;
	};

	inline bool operator==( const String & p_lhs, const String & p_rhs )
	{
		return String::MyString( p_lhs ) == String::MyString( p_rhs );
	}

	inline bool operator==( const String & p_lhs, const std::string & p_rhs )
	{
		return p_lhs == String( p_rhs );
	}

	inline bool operator==( const String & p_lhs, const std::wstring & p_rhs )
	{
		return p_lhs == String( p_rhs );
	}

	inline bool operator==( const std::string & p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	inline bool operator==( const std::wstring & p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	inline bool operator==( const String & p_lhs, const char * p_rhs )
	{
		return p_lhs == String( p_rhs );
	}

	inline bool operator==( const String & p_lhs, const wchar_t * p_rhs )
	{
		return p_lhs == String( p_rhs );
	}

	inline bool operator==( const char * p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	inline bool operator==( const wchar_t * p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	template< size_t N >
	inline bool operator==( const String & p_lhs, const char p_rhs[N] )
	{
		return p_lhs == String( p_rhs );
	}

	template< size_t N >
	inline bool operator==( const String & p_lhs, const wchar_t p_rhs[N] )
	{
		return p_lhs == String( p_rhs );
	}

	template< size_t N >
	inline bool operator==( const char p_lhs[N], const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	template< size_t N >
	inline bool operator==( const wchar_t p_lhs[N], const String & p_rhs )
	{
		return String( p_lhs ) == p_rhs;
	}

	inline bool operator!=( const String & p_lhs, const String & p_rhs )
	{
		return String::MyString( p_lhs ) != String::MyString( p_rhs );
	}

	inline bool operator!=( const String & p_lhs, const std::string & p_rhs )
	{
		return p_lhs != String( p_rhs );
	}

	inline bool operator!=( const String & p_lhs, const std::wstring & p_rhs )
	{
		return p_lhs != String( p_rhs );
	}

	inline bool operator!=( const std::string & p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) != p_rhs;
	}

	inline bool operator!=( const std::wstring & p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) != p_rhs;
	}

	inline bool operator!=( const String & p_lhs, const char * p_rhs )
	{
		return p_lhs != String( p_rhs );
	}

	inline bool operator!=( const String & p_lhs, const wchar_t * p_rhs )
	{
		return p_lhs != String( p_rhs );
	}

	inline bool operator!=( const char * p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) != p_rhs;
	}

	inline bool operator!=( const wchar_t * p_lhs, const String & p_rhs )
	{
		return String( p_lhs ) != p_rhs;
	}

	inline String operator+( const String & p_lhs, const String & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const String & p_lhs, const std::string & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const String & p_lhs, const std::wstring & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const String & p_lhs, const char * p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const String & p_lhs, const wchar_t * p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const std::string & p_lhs, const String & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const std::wstring & p_lhs, const String & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const char * p_lhs, const String & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	inline String operator+( const wchar_t * p_lhs, const String & p_rhs )
	{
		String l_tmp( p_lhs );
		l_tmp += p_rhs;
		return l_tmp;
	}

	template <typename T>
	inline String ToString( const T & p_tValue)
	{
		String l_strReturn;
		l_strReturn.Parse( p_tValue);
		return l_strReturn;
	}

	static String C3DEmptyString;
}
}

#endif
