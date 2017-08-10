/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CastorTest_UnitTest___
#define ___CastorTest_UnitTest___

#include "CastorTestPrerequisites.hpp"

namespace Testing
{
	template< typename T >
	inline std::string toString( T const & p_value )
	{
		std::stringstream stream;
		stream << p_value;
		return stream.str();
	}

	template<>
	inline std::string toString< std::chrono::milliseconds >( std::chrono::milliseconds const & p_value )
	{
		std::stringstream stream;
		stream << p_value.count() << "ms";
		return stream.str();
	}

	template<>
	inline std::string toString< std::chrono::microseconds >( std::chrono::microseconds const & p_value )
	{
		std::stringstream stream;
		stream << p_value.count() << "us";
		return stream.str();
	}

	template<>
	inline std::string toString< std::chrono::nanoseconds >( std::chrono::nanoseconds const & p_value )
	{
		std::stringstream stream;
		stream << p_value.count() << "ns";
		return stream.str();
	}

	template<>
	inline std::string toString< std::wstring >( std::wstring const & p_value )
	{
		std::stringstream stream;
		stream << p_value.c_str();
		return stream.str();
	}

	template< typename T >
	inline std::string toString( std::pair< T const *, uint32_t > const & p_value )
	{
		std::stringstream stream;
		stream << p_value.second << ": ";

		std::for_each( p_value.first, p_value.first + p_value.second, [&stream]( T const & p_val )
		{
			stream << " " << p_val;
		} );

		return stream.str();
	}

	inline std::string toString( std::pair< uint8_t const *, uint32_t > const & p_value )
	{
		std::stringstream stream;
		stream << p_value.second << ": ";

		std::for_each( p_value.first, p_value.first + p_value.second, [&stream]( uint8_t const & p_val )
		{
			stream << " " << std::hex << uint16_t( p_val );
		} );

		return stream.str();
	}

	template< class Value >
	class Lazy
	{
		using value_type = Value;
		using pointer_type = std::shared_ptr< value_type >;
		typedef std::function< pointer_type() > getter;

	public:
		Lazy( std::function< Value() > const & p_expression )
			: m_thunk{ std::make_shared< getter >( [p_expression]()
			{
				return std::make_shared< value_type >( p_expression() );
			} ) }
		{
		}

		Lazy( Lazy const & ) = delete;

		const Value & operator()()const
		{
			pointer_type value{ ( *m_thunk )() };

			*m_thunk = [value]()
			{
				return value;
			};

			return *value;
		}

		Value & operator()()
		{
			pointer_type value{ ( *m_thunk )() };

			*m_thunk = [value]()
			{
				return value;
			};

			return *value;
		}

	private:
		mutable std::shared_ptr< getter > m_thunk;
	};

	template< class Value >
	class Lazy< Value & >
	{
		using value_type = std::reference_wrapper< Value >;
		using pointer_type = std::shared_ptr< value_type >;
		typedef std::function< pointer_type() > getter;

	public:
		Lazy( std::function< Value &() > const & p_expression )
			: m_thunk{ std::make_shared< getter >( [p_expression]()
			{
				return std::make_shared< value_type >( std::ref( p_expression() ) );
			} ) }
		{
		}

		Lazy( Lazy const & ) = delete;

		const Value & operator()()const
		{
			pointer_type value{ ( *m_thunk )() };

			*m_thunk = [value]()
			{
				return value;
			};

			return value->get();
		}

		Value & operator()()
		{
			pointer_type value{ ( *m_thunk )() };

			*m_thunk = [value]()
			{
				return value;
			};

			return value->get();
		}

	private:
		mutable std::shared_ptr< getter > m_thunk;
	};

	template<>
	class Lazy< void >
	{
		using value_type = void;
		typedef std::function< void() > getter;

	public:
		Lazy( std::function< void() > const & p_expression )
			: m_thunk{ std::make_shared< getter >( [p_expression]()
		{
			return p_expression();
		} ) }
		{
		}

		Lazy( Lazy const & ) = delete;

		void operator()()const
		{
			return ( *m_thunk )();
		}

		void operator()()
		{
			return ( *m_thunk )();
		}

	private:
		mutable std::shared_ptr< getter > m_thunk;
	};

	class TestFailed
		: public std::exception
	{
	public:
		TestFailed( std::string const & p_what
			, std::string const & p_file
			, std::string const & p_function
			, int p_line );
		virtual ~TestFailed() throw( );
		const char * what()
		{
			return m_what.c_str();
		}

	private:
		std::string m_what;
	};

	class TestCase
	{
	public:
		using TestFunction = std::function< void () >;

	public:
		explicit TestCase( std::string const & p_name );
		virtual ~TestCase();
		void RegisterTests();
		void Execute( uint32_t & p_errCount
			, uint32_t & p_testCount );

		inline std::string const & getName()const
		{
			return m_name;
		}

		inline void ReportFailure()
		{
			( *m_errorCount )++;
		}

		inline void addTest()
		{
			( *m_testCount )++;
		}

	protected:
		void doRegisterTest( std::string const & p_name
			, TestFunction p_test );

	private:
		virtual void doRegisterTests() = 0;

	protected:
		template< typename Type >
		inline bool Check( Lazy< Type > const & p_condition
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const condition = p_condition();

				if ( !condition )
				{
					ReportFailure();
					std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
				}
				else
				{
					result = true;
				}
			}
			catch ( std::exception & p_exc )
			{
				std::cerr << "Uncaught Exception: " << p_exc.what() << std::endl;
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << "(Unexpected exception)" << std::endl;
			}
			catch ( ... )
			{
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << "(Unexpected exception)" << std::endl;
			}

			return result;
		}

		template< typename Type >
		inline bool Require( Lazy< Type > const & p_condition
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const condition = p_condition();

				if ( !condition )
				{
					throw TestFailed( p_conditionName, p_file, p_function, p_line );
				}
			}
			catch ( std::exception & p_exc )
			{
				std::cerr << "Uncaught Exception: " << p_exc.what() << std::endl;
				throw TestFailed( p_conditionName, p_file, p_function, p_line );
			}
			catch ( ... )
			{
				throw TestFailed( p_conditionName, p_file, p_function, p_line );
			}

			return result;
		}

		template< typename Type >
		inline bool CheckThrow( Lazy< Type > const & p_condition
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				p_condition();
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
			}
			catch ( ... )
			{
				result = true;
			}

			return result;
		}

		template< typename Type >
		inline bool CheckNoThrow( Lazy< Type > const & p_condition
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				p_condition();
				result = true;
			}
			catch ( std::exception & p_exc )
			{
				std::cerr << "Uncaught Exception: " << p_exc.what() << std::endl;
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
			}
			catch ( ... )
			{
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		inline bool CheckEqual( ComparatorType p_compare
			, Lazy< LhsType > const & p_lhs
			, Lazy< RhsType > const & p_rhs
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_lhsName
			, char const * const p_rhsName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const & lhs = p_lhs();
				auto const & rhs = p_rhs();
				result = p_compare( lhs, rhs );

				if ( !result )
				{
					ReportFailure();
					std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << " (" << ::Testing::toString( lhs ) << " != " << ::Testing::toString( rhs ) << ")" << std::endl;
				}
			}
			catch ( std::exception & p_exc )
			{
				std::cerr << "Uncaught Exception: " << p_exc.what() << std::endl;
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << "(Unexpected exception)" << std::endl;
			}
			catch ( ... )
			{
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << "(Unexpected exception)" << std::endl;
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		inline bool CheckNotEqual( ComparatorType p_compare
			, Lazy< LhsType > const & p_lhs
			, Lazy< RhsType > const & p_rhs
			, char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_lhsName
			, char const * const p_rhsName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const & lhs = p_lhs();
				auto const & rhs = p_rhs();
				result = p_compare( lhs, rhs );

				if ( result )
				{
					ReportFailure();
					std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << " (" << ::Testing::toString( lhs ) << " == " << ::Testing::toString( rhs ) << ")" << std::endl;
				}
			}
			catch ( std::exception & p_exc )
			{
				std::cerr << "Uncaught Exception: " << p_exc.what() << std::endl;
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << "(Unexpected exception)" << std::endl;
			}
			catch ( ... )
			{
				ReportFailure();
				std::cerr << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << "(Unexpected exception)" << std::endl;
			}

			return result;
		}

		template< typename T, typename U >
		inline bool compare( T const & p_lhs, U const & p_rhs )
		{
			return p_lhs == p_rhs;
		}

		template< typename T >
		inline bool compare( std::pair< T const *, uint32_t > const & p_a, std::pair< T const *, uint32_t > p_b )
		{
			bool result = p_a.second == p_b.second;

			if ( result )
			{
				result = std::memcmp( p_a.first, p_b.first, p_a.second * sizeof( T ) ) == 0;
			}

			return result;
		}

		inline bool compare( float const & p_a, float const & p_b )
		{
			float epsilon = float ( 0.0001 );
			return std::abs( float ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		inline bool compare( float const & p_a, double const & p_b )
		{
			float epsilon = float ( 0.0001 );
			return std::abs( float ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		inline bool compare( double const & p_a, double const & p_b )
		{
			double epsilon = double ( 0.0001 );
			return std::abs( double ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		inline bool compare( double const & p_a, float const & p_b )
		{
			double epsilon = double( 0.0001 );
			return std::abs( double( p_a - p_b ) ) < epsilon
				|| ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		template< typename T, size_t C1, size_t C2 >
		inline bool compare( std::array< T, C1 > const & p_a, std::array< T, C2 > const & p_b )
		{
			bool result{ C1 == C2 };

			for ( size_t i = 0u; i < C1 && result; ++i )
			{
				result = this->compare( p_a[i], p_b[i] );
			}

			return result;
		}

	private:
		uint32_t * m_errorCount{ nullptr };
		uint32_t * m_testCount{ nullptr };
		std::string m_name;
		std::vector< std::pair< std::string, TestFunction > > m_tests;
	};

#	define LAZY( E ) ::Testing::Lazy< decltype( ( E ) ) >( std::function< decltype( ( E ) )() >( [&]() -> decltype( ( E ) )\
	{\
		return E;\
	} ) )

#	define CT_CHECK( x )\
	Check( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_EQUAL( x, y )\
	CheckEqual( [&]( auto const & lhs, auto const & rhs ){ return this->compare( lhs, rhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, __LINE__, #x, #y )

#	define CT_NEQUAL( x, y )\
	CheckNotEqual( [&]( auto const & lhs, auto const & rhs ){ return this->compare( lhs, rhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, __LINE__, #x, #y )

#	define CT_CHECK_THROW( x )\
	CheckThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_CHECK_NOTHROW( x )\
	CheckNoThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_REQUIRE( x )\
	Require( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )
}

#endif
