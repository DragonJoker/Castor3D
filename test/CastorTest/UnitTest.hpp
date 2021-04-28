/* See LICENSE file in root folder */
#ifndef ___CastorTest_UnitTest___
#define ___CastorTest_UnitTest___

#include "CastorTestPrerequisites.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <map>

namespace Testing
{
	template< typename T >
	inline std::string toString( T const & p_value )
	{
		std::stringstream stream;
		stream << p_value;
		return stream.str();
	}

	inline std::string toString( std::nullptr_t const & )
	{
		return "nullptr";
	}

	template<>
	inline std::string toString< std::chrono::seconds >( std::chrono::seconds const & p_value )
	{
		std::stringstream stream;
		stream << p_value.count() << "s";
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
			stream << " " << toString( p_val );
		} );

		return stream.str();
	}

	template< typename T >
	inline std::string toString( std::pair< T *, uint32_t > const & p_value )
	{
		std::stringstream stream;
		stream << p_value.second << ": ";

		std::for_each( p_value.first, p_value.first + p_value.second, [&stream]( T const & p_val )
		{
			stream << " " << toString( p_val );
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

	template< typename Key, typename Value >
	inline std::string toString( std::map< Key, Value > const & p_value )
	{
		std::stringstream stream;
		stream << std::endl << "[" << std::endl;

		for ( auto & pair : p_value )
		{
			stream << "  [" << toString( pair.first ) << ": " << toString( pair.second ) << "]" << std::endl;
		}

		stream << "]" << std::endl;
		return stream.str();
	}

	template< typename Key, typename Value >
	inline std::string toString( std::map< Key, std::shared_ptr< Value > > const & p_value )
	{
		std::stringstream stream;
		stream << std::endl << "[" << std::endl;

		for ( auto & pair : p_value )
		{
			stream << "  [" << toString( pair.first ) << ": " << toString( *pair.second ) << "]" << std::endl;
		}

		stream << "]" << std::endl;
		return stream.str();
	}

	template< class Value >
	class Lazy
	{
		using value_type = Value;
		using pointer_type = std::shared_ptr< value_type >;
		using getter = std::function< pointer_type() >;

	public:
		explicit Lazy( std::function< Value() > const & p_expression )
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
		using getter = std::function< pointer_type() >;

	public:
		explicit Lazy( std::function< Value &() > const & p_expression )
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
		using getter = std::function< void() >;

	public:
		explicit Lazy( std::function< void() > const & p_expression )
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

	struct TestBlock
	{
		TestBlock( TestCase & testCase
			, std::string const & text );
		~TestBlock();

		TestCase & testCase;
		std::string text;
	};

	using TestBlockPtr = std::unique_ptr< TestBlock >;

	class TestCase
	{
	public:
		using TestFunction = std::function< void () >;
		friend struct TestBlock;

	public:
		explicit TestCase( std::string const & p_name );
		virtual ~TestCase();
		void registerTests();
		void execute( uint32_t & p_errCount
			, uint32_t & p_testCount );

		TestBlockPtr on( std::string const & text )
		{
			return doPushBlock( "On " + text );
		}

		TestBlockPtr when( std::string text )
		{
			return doPushBlock( "When " + text );
		}

		std::string const & getName()const
		{
			return m_name;
		}

		void reportFailure()
		{
			( *m_errorCount )++;
		}

		void addTest()
		{
			( *m_testCount )++;
		}

	protected:
		void doRegisterTest( std::string const & p_name
			, TestFunction p_test );

	private:
		virtual void doRegisterTests() = 0;

		TestBlockPtr doPushBlock( std::string const & text );
		void doPopBlock( TestBlock * block );
		void doPrintError( std::string const & error );

	public:
		void fail( char const * const p_file
			, char const * const p_function
			, uint32_t const p_line
			, char const * const p_conditionName )
		{
			reportFailure();
			std::stringstream err;
			err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
			doPrintError( err.str() );
		}

		template< typename Type >
		bool check( Lazy< Type > const & p_condition
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
					reportFailure();
					std::stringstream err;
					err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
					doPrintError( err.str() );
				}
				else
				{
					result = true;
				}
			}
			catch ( std::exception & p_exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << p_exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename Type >
		bool require( Lazy< Type > const & p_condition
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
				std::stringstream err;
				err << "Uncaught Exception: " << p_exc.what() << std::endl;
				doPrintError( err.str() );
				throw TestFailed( p_conditionName, p_file, p_function, p_line );
			}
			catch ( ... )
			{
				throw TestFailed( p_conditionName, p_file, p_function, p_line );
			}

			return result;
		}

		template< typename Type >
		bool checkThrow( Lazy< Type > const & p_condition
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
				reportFailure();
				std::stringstream err;
				err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
				doPrintError( err.str() );
			}
			catch ( ... )
			{
				result = true;
			}

			return result;
		}

		template< typename Type >
		bool checkNoThrow( Lazy< Type > const & p_condition
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
				std::stringstream err1;
				err1 << "Uncaught Exception: " << p_exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		bool checkEqual( ComparatorType p_compare
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
					reportFailure();
					std::stringstream err;
					err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << " (" << ::Testing::toString( lhs ) << " != " << ::Testing::toString( rhs ) << ")" << std::endl;
					doPrintError( err.str() );
				}
			}
			catch ( std::exception & p_exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << p_exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		bool checkNotEqual( ComparatorType p_compare
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
					reportFailure();
					std::stringstream err;
					err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << " (" << ::Testing::toString( lhs ) << " == " << ::Testing::toString( rhs ) << ")" << std::endl;
					doPrintError( err.str() );
				}
			}
			catch ( std::exception & p_exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << p_exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " != " << p_rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename T, typename U >
		bool compare( T const & p_lhs, U const & p_rhs )
		{
			return p_lhs == p_rhs;
		}

		template< typename T >
		bool compare( std::pair< T const *, uint32_t > const & p_a, std::pair< T const *, uint32_t > p_b )
		{
			bool result = p_a.second == p_b.second;

			if ( result )
			{
				result = std::memcmp( p_a.first, p_b.first, p_a.second * sizeof( T ) ) == 0;
			}

			return result;
		}

		template< typename T >
		bool compare( std::pair< T *, uint32_t > const & p_a, std::pair< T *, uint32_t > p_b )
		{
			bool result = p_a.second == p_b.second;

			if ( result )
			{
				result = std::memcmp( p_a.first, p_b.first, p_a.second * sizeof( T ) ) == 0;
			}

			return result;
		}

		template< typename Key, typename Value >
		bool compare( std::map< Key, Value > const & p_a, std::map< Key, Value > p_b )
		{
			bool result = p_a.size() == p_b.size();

			for ( auto & itA : p_a )
			{
				if ( result )
				{
					auto itB = p_b.find( itA.first );
					result = itB != p_b.end();
					result &= compare( itA.second, itB->second );
				}
			}

			return result;
		}

		template< typename Key, typename Value >
		bool compare( std::map< Key, std::shared_ptr< Value > > const & p_a, std::map< Key, std::shared_ptr< Value > > p_b )
		{
			bool result = p_a.size() == p_b.size();

			for ( auto & itA : p_a )
			{
				if ( result )
				{
					auto itB = p_b.find( itA.first );
					result = itB != p_b.end();
					result &= compare( *itA.second, *itB->second );
				}
			}

			return result;
		}

		bool compare( float const & p_a, float const & p_b )
		{
			float epsilon = float ( 0.0001 );
			return std::abs( float ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		bool compare( float const & p_a, double const & p_b )
		{
			float epsilon = float ( 0.0001 );
			return std::abs( float ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		bool compare( double const & p_a, double const & p_b )
		{
			double epsilon = double ( 0.0001 );
			return std::abs( double ( p_a - p_b ) ) < epsilon
				   || ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		bool compare( double const & p_a, float const & p_b )
		{
			double epsilon = double( 0.0001 );
			return std::abs( double( p_a - p_b ) ) < epsilon
				|| ( std::isnan( p_a ) && std::isnan( p_b ) );
		}

		template< typename T, size_t C1, size_t C2 >
		bool compare( std::array< T, C1 > const & p_a, std::array< T, C2 > const & p_b )
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
		std::vector< TestBlock * > m_blocks;
	};

#	define LAZY( E ) ::Testing::Lazy< decltype( ( E ) ) >( std::function< decltype( ( E ) )() >( [&]() -> decltype( ( E ) )\
	{\
		return E;\
	} ) )

#	define CT_FAILURE_EX( test, x )\
	( test ).fail( __FILE__, __FUNCTION__, __LINE__, x )

#	define CT_CHECK_EX( test, x )\
	( test ).check( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_EQUAL_EX( test, x, y )\
	( test ).checkEqual( [&]( auto const & lhs, auto const & rhs ){ return ( test ).compare( lhs, rhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x, #y )

#	define CT_NEQUAL_EX( test, x, y )\
	( test ).checkNotEqual( [&]( auto const & lhs, auto const & rhs ){ return ( test ).compare( lhs, rhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x, #y )

#	define CT_CHECK_THROW_EX( test, x )\
	( test ).checkThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_CHECK_NOTHROW_EX( test, x )\
	( test ).checkNoThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_REQUIRE_EX( test, x )\
	( test ).require( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_ON_EX( test, text )\
	Testing::TestBlockPtr block{ ( test ).on( text ) }

#	define CT_WHEN_EX( test, text )\
	Testing::TestBlockPtr block{ ( test ).when( text ) }

#	define CT_FAILURE( x )\
	CT_FAILURE_EX( *this, x )

#	define CT_CHECK( x )\
	CT_CHECK_EX( *this, x )

#	define CT_EQUAL( x, y )\
	CT_EQUAL_EX( *this, x, y )

#	define CT_NEQUAL( x, y )\
	CT_NEQUAL_EX( *this, x, y )

#	define CT_CHECK_THROW( x )\
	CT_CHECK_THROW_EX( *this, x )

#	define CT_CHECK_NOTHROW( x )\
	CT_CHECK_NOTHROW_EX( *this, x )

#	define CT_REQUIRE( x )\
	CT_REQUIRE_EX( *this, x )

#	define CT_ON( text )\
	CT_ON_EX( *this, text )

#	define CT_WHEN( text )\
	CT_WHEN_EX( *this, text )
}

#endif
