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
#ifndef ___CUT_UnitTest___
#define ___CUT_UnitTest___

#include "TestPrerequisites.hpp"

namespace Testing
{
	template< typename T >
	static inline std::string to_string( T const & p_value )
	{
		std::stringstream l_stream;
		l_stream << p_value;
		return l_stream.str();
	}

	class TestCase
	{
	private:
		std::string	m_strName;

	public:
		TestCase( std::string const & p_strName );
		virtual ~TestCase();
		virtual void Execute( uint32_t & p_errCount, uint32_t & p_testCount ) = 0;
		inline std::string const & GetName()const
		{
			return m_strName;
		}
	};

	DECLARE_SMART_PTR( TestCase );

	class TestFailed
		:	public std::exception
	{
	public:
		TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line );
		virtual ~TestFailed() throw();
		const char * what()
		{
			return m_what.c_str();
		}

	private:
		std::string m_what;
	};

#	define TEST_CHECK( x ) p_testCount++;if( !(x) ) { p_errCount++;Castor::Logger::LogWarning( "%s - %s, line %d : %s", __FILE__, __FUNCTION__, __LINE__, #x ); }
#	define TEST_EQUAL( x, y ) p_testCount++;if( !(x == y) ) { p_errCount++;Castor::Logger::LogWarning( "%s - %s, line %d : %s == %s (%s != %s)", __FILE__, __FUNCTION__, __LINE__, std::string( #x ).c_str(), std::string( #y ).c_str(), Testing::to_string( x ).c_str(), Testing::to_string( y ).c_str() ); }
#	define TEST_REQUIRE( x ) p_testCount++;if( !(x) ) { p_errCount++;throw TestFailed( #x, __FILE__, __FUNCTION__, __LINE__ ); }
}

#endif
