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
	inline std::string to_string( T const & p_value )
	{
		std::stringstream l_stream;
		l_stream << p_value;
		return l_stream.str();
	}
	template<>
	inline std::string to_string< Castor::String >( Castor::String const & p_value )
	{
		std::stringstream l_stream;
		l_stream << Castor::string::to_str( p_value );
		return l_stream.str();
	}

	class TestCase
	{
	public:
		TestCase( std::string const & p_strName );
		virtual ~TestCase();
		virtual void Execute( uint32_t & p_errCount, uint32_t & p_testCount ) = 0;
		inline std::string const & GetName()const
		{
			return m_strName;
		}

	protected:
		void DoExecuteTest( std::function< void( uint32_t &, uint32_t & ) > p_test, uint32_t & p_errCount, uint32_t & p_testCount, std::string const & p_name );

	private:
		std::string	m_strName;
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

#	define TEST_CHECK( x )\
	p_testCount++;\
	if( !(x) )\
	{\
		p_errCount++;\
		Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x );\
	}

#	define TEST_EQUAL( x, y )\
	p_testCount++;\
	if( !(x == y) )\
	{\
		p_errCount++;\
		Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x << " == " << #y << " (" << Testing::to_string( x ) << " != " << Testing::to_string( y ) << ")" );\
	}

#	define TEST_REQUIRE( x )\
	p_testCount++;\
	if( !(x) )\
	{\
		p_errCount++;\
		throw TestFailed( #x, __FILE__, __FUNCTION__, __LINE__ );\
	}

#	define EXECUTE_TEST( class_name, test_func, errors, tests )\
	DoExecuteTest( std::bind( &class_name::test_func, this, std::placeholders::_1, std::placeholders::_2 ), errors, tests, #test_func );
}

#endif
