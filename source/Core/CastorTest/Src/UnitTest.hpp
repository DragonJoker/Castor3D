/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___CastorTest_UnitTest___
#define ___CastorTest_UnitTest___

#include "TestPrerequisites.hpp"

#include <cmath>

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
	inline std::string to_string< std::wstring >( std::wstring const & p_value )
	{
		std::stringstream l_stream;
		l_stream << Castor::string::string_cast< char >( p_value );
		return l_stream.str();
	}

	template< typename T >
	inline std::string to_string( std::pair< T const *, uint32_t > const & p_value )
	{
		std::stringstream l_stream;
		l_stream << p_value.second << ": ";

		std::for_each( p_value.first, p_value.first + p_value.second, [&l_stream]( T const & p_val )
		{
			l_stream << " " << p_val;
		} );

		return l_stream.str();
	}

	template< typename T, typename U >
	inline bool compare( T const & p_a, U const & p_b )
	{
		return p_a == p_b;
	}

	template< typename T >
	inline bool compare( std::pair< T const *, uint32_t > const & p_a, std::pair< T const *, uint32_t > p_b )
	{
		bool l_return = p_a.second == p_b.second;

		if ( l_return )
		{
			l_return = std::memcmp( p_a.first, p_b.first, p_a.second * sizeof( T ) ) == 0;
		}

		return l_return;
	}

	template<>
	inline bool compare< float, float >( float const & p_a, float const & p_b )
	{
		float l_epsilon = float( 0.0001 );
		return std::abs( float( p_a - p_b ) ) < l_epsilon;
	}

	template<>
	inline bool compare< float, double >( float const & p_a, double const & p_b )
	{
		float l_epsilon = float( 0.0001 );
		return std::abs( float( p_a - p_b ) ) < l_epsilon;
	}

	template<>
	inline bool compare< double, double >( double const & p_a, double const & p_b )
	{
		double l_epsilon = double( 0.0001 );
		return std::abs( double( p_a - p_b ) ) < l_epsilon;
	}

	template<>
	inline bool compare< double, float >( double const & p_a, float const & p_b )
	{
		double l_epsilon = double( 0.0001 );
		return std::abs( double( p_a - p_b ) ) < l_epsilon;
	}

	class TestCase
	{
	public:
		using TestFunction = std::function< void() >;

	public:
		explicit TestCase( std::string const & p_name );
		virtual ~TestCase();
		void RegisterTests();
		void Execute( uint32_t & p_errCount, uint32_t & p_testCount );

		inline std::string const & GetName()const
		{
			return m_name;
		}

		inline void ReportFailure()
		{
			( *m_errorCount )++;
		}

		inline void AddTest()
		{
			( *m_testCount )++;
		}

	protected:
		void DoRegisterTest( std::string const & p_name, TestFunction p_test );

	private:
		virtual void DoRegisterTests() = 0;

	protected:

	private:
		uint32_t * m_errorCount{ nullptr };
		uint32_t * m_testCount{ nullptr };
		std::string m_name;
		std::vector< std::pair< std::string, TestFunction > > m_tests;
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

#	define CT_CHECK( x )\
	[&]()\
	{\
		AddTest();\
		bool l_check{ x };\
		if( !l_check )\
		{\
			ReportFailure();\
			Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x );\
		}\
		return l_check;\
	}()

#	define CT_EQUAL( x, y )\
	[&]()\
	{\
		AddTest();\
		bool l_check{ compare( x, y ) };\
		if( !l_check )\
		{\
			ReportFailure();\
			Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x << " == " << #y << " (" << ::Testing::to_string( x ) << " != " << ::Testing::to_string( y ) << ")" );\
		}\
		return l_check;\
	}()

#	define CT_CHECK_THROW( x )\
	AddTest();\
	try\
	{\
		( x ); \
		ReportFailure();\
		Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x );\
	}\
	catch ( ... )\
	{\
	}

#	define CT_CHECK_NOTHROW( x )\
	AddTest();\
	try\
	{\
		( x ); \
	}\
	catch ( ... )\
	{\
		ReportFailure();\
		Castor::Logger::LogWarning( std::stringstream() << "Failure at " << __FILE__ << " - " << __FUNCTION__ << ", line " << __LINE__ << ": " << #x );\
	}

#	define CT_REQUIRE( x )\
	AddTest();\
	if( !(x) )\
	{\
		throw TestFailed( #x, __FILE__, __FUNCTION__, __LINE__ );\
	}
}

#endif
