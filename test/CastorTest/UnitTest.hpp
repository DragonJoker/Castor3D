/* See LICENSE file in root folder */
#ifndef ___CastorTest_UnitTest___
#define ___CastorTest_UnitTest___

#include "CastorTestPrerequisites.hpp"

#include <array>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Testing
{
	template< typename ValueT >
	struct Stringifier;

	inline std::string toString( std::nullptr_t const & )
	{
		return "nullptr";
	}

	template< typename ValueT >
	inline std::string toString( ValueT const & value )
	{
		return Stringifier< ValueT >::get( value );
	}

	template<>
	struct Stringifier< bool >
	{
		static std::string get( bool const & value )
		{
			std::stringstream stream;
			stream << ( value ? std::string{ "true" } : std::string{ "false" } );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< char >
	{
		static std::string get( char const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< int8_t >
	{
		static std::string get( int8_t const & value )
		{
			std::stringstream stream;
			stream << int16_t( value );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< uint8_t >
	{
		static std::string get( uint8_t const & value )
		{
			std::stringstream stream;
			stream << uint16_t( value );
			return stream.str();
		}
	};

	template<>
	struct Stringifier< int16_t >
	{
		static std::string get( int16_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< uint16_t >
	{
		static std::string get( uint16_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< int32_t >
	{
		static std::string get( int32_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< uint32_t >
	{
		static std::string get( uint32_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< int64_t >
	{
		static std::string get( int64_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< uint64_t >
	{
		static std::string get( uint64_t const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< float >
	{
		static std::string get( float const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< double >
	{
		static std::string get( double const & value )
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::chrono::seconds >
	{
		static std::string get( std::chrono::seconds const & value )
		{
			std::stringstream stream;
			stream << toString( value.count() ) << " s";
			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::chrono::milliseconds >
	{
		static std::string get( std::chrono::milliseconds const & value )
		{
			std::stringstream stream;
			stream << toString( value.count() ) << " ms";
			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::chrono::microseconds >
	{
		static std::string get( std::chrono::microseconds const & value )
		{
			std::stringstream stream;
			stream << toString( value.count() ) << " us";
			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::chrono::nanoseconds >
	{
		static std::string get( std::chrono::nanoseconds const & value )
		{
			std::stringstream stream;
			stream << toString( value.count() ) << " ns";
			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::string >
	{
		static std::string get( std::string const & value )
		{
			return value;
		}
	};

	template<>
	struct Stringifier< std::wstring >
	{
		static std::string get( std::wstring const & value )
		{
			std::stringstream stream;
			stream << reinterpret_cast< char const * >( value.c_str() );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< ValueT * >
	{
		static std::string get( ValueT * value )
		{
			std::stringstream stream;
			stream << ( value ? toString( *value ) : std::string{ "null_rptr" } );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< ValueT const * >
	{
		static std::string get( ValueT const * value )
		{
			std::stringstream stream;
			stream << ( value ? toString( *value ) : std::string{ "null_rptr" } );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::shared_ptr< ValueT > >
	{
		static std::string get( std::shared_ptr< ValueT > const & value )
		{
			std::stringstream stream;
			stream << ( value ? toString( *value ) : std::string{ "null_sptr" } );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::unique_ptr< ValueT > >
	{
		static std::string get( std::unique_ptr< ValueT > const & value )
		{
			std::stringstream stream;
			stream << ( value ? toString( *value ) : std::string{ "null_sptr" } );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::weak_ptr< ValueT > >
	{
		static std::string get( std::weak_ptr< ValueT > const & value )
		{
			std::stringstream stream;
			stream << ( value.expired() ? std::string{ "null_wptr" } : toString( value.lock() ) );
			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::pair< ValueT const *, uint32_t > >
	{
		static std::string get( std::pair< ValueT const *, uint32_t > const & value )
		{
			std::stringstream stream;
			stream << value.second << ":";

			std::for_each( value.first
				, value.first + value.second
				, [&stream]( ValueT const & val )
				{
					stream << " " << toString( val );
				} );

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::pair< ValueT *, uint32_t > >
	{
		static std::string get( std::pair< ValueT *, uint32_t > const & value )
		{
			std::stringstream stream;
			stream << value.second << ":";

			std::for_each( value.first
				, value.first + value.second
				, [&stream]( ValueT const & val )
				{
					stream << " " << toString( val );
				} );

			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::pair< uint8_t const *, uint32_t > >
	{
		static std::string get( std::pair< uint8_t const *, uint32_t > const & value )
		{
			std::stringstream stream;
			stream << value.second << ":";

			std::for_each( value.first
				, value.first + value.second
				, [&stream]( uint8_t const & val )
				{
					stream << " " << std::hex << uint16_t( val );
				} );

			return stream.str();
		}
	};

	template<>
	struct Stringifier< std::pair< uint8_t *, uint32_t > >
	{
		static std::string get( std::pair< uint8_t *, uint32_t > const & value )
		{
			std::stringstream stream;
			stream << value.second << ":";

			std::for_each( value.first
				, value.first + value.second
				, [&stream]( uint8_t const & val )
				{
					stream << " " << std::hex << uint16_t( val );
				} );

			return stream.str();
		}
	};

	template< typename ValueT, size_t CountT >
	struct Stringifier< std::array< ValueT, CountT > >
	{
		static std::string get( std::array< ValueT, CountT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename ValueT, size_t CountT >
	using CArrayType = ValueT[CountT];

	template< typename ValueT, size_t CountT >
	struct Stringifier< CArrayType< ValueT, CountT > >
	{
		static std::string get( CArrayType< ValueT, CountT > const & values )
		{
			std::stringstream stream;
			stream << CountT << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::vector< ValueT > >
	{
		static std::string get( std::vector< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::set< ValueT > >
	{
		static std::string get( std::set< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::unordered_set< ValueT > >
	{
		static std::string get( std::unordered_set< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename ValueT >
	struct Stringifier< std::list< ValueT > >
	{
		static std::string get( std::list< ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << " " << toString( value );
			}

			return stream.str();
		}
	};

	template< typename KeyT, typename ValueT >
	struct Stringifier< std::map< KeyT, ValueT > >
	{
		static std::string get( std::map< KeyT, ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << "  [" << toString( value.first ) << ": " << toString( value.second ) << "]" << std::endl;
			}

			return stream.str();
		}
	};

	template< typename KeyT, typename ValueT >
	struct Stringifier< std::unordered_map< KeyT, ValueT > >
	{
		static std::string get( std::unordered_map< KeyT, ValueT > const & values )
		{
			std::stringstream stream;
			stream << values.size() << ":";

			for ( auto & value : values )
			{
				stream << "  [" << toString( value.first ) << ": " << toString( value.second ) << "]" << std::endl;
			}

			return stream.str();
		}
	};

	template< class Value >
	class Lazy
	{
		using value_type = Value;
		using pointer_type = std::shared_ptr< value_type >;
		using getter = std::function< pointer_type() >;

	public:
		explicit Lazy( std::function< Value() > const & expression )
			: m_thunk{ std::make_shared< getter >( [expression]()
			{
				return std::make_shared< value_type >( expression() );
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
		explicit Lazy( std::function< Value &() > const & expression )
			: m_thunk{ std::make_shared< getter >( [expression]()
			{
				return std::make_shared< value_type >( std::ref( expression() ) );
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
		explicit Lazy( std::function< void() > const & expression )
			: m_thunk{ std::make_shared< getter >( [expression]()
			{
				return expression();
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
		TestFailed( std::string const & what
			, std::string const & file
			, std::string const & function
			, uint32_t line );

		const char * what()const noexcept override
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
		explicit TestCase( std::string const & name );
		virtual ~TestCase();
		void registerTests();
		void execute( uint32_t & errCount
			, uint32_t & testCount );

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
		virtual void doRegisterTest( std::string const & name
			, TestFunction test );

	private:
		virtual void doRegisterTests() = 0;

		TestBlockPtr doPushBlock( std::string const & text );
		void doPopBlock( TestBlock * block );
		void doPrintError( std::string const & error );

	public:
		void fail( char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const conditionName )
		{
			reportFailure();
			std::stringstream err;
			err << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << std::endl;
			doPrintError( err.str() );
		}

		template< typename Type >
		bool check( Lazy< Type > const & condition
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const cond = condition();

				if ( !cond )
				{
					reportFailure();
					std::stringstream err;
					err << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << std::endl;
					doPrintError( err.str() );
				}
				else
				{
					result = true;
				}
			}
			catch ( std::exception & exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename Type >
		bool require( Lazy< Type > const & condition
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const cond = condition();

				if ( !cond )
				{
					throw TestFailed( conditionName, file, function, line );
				}
			}
			catch ( std::exception & exc )
			{
				std::stringstream err;
				err << "Uncaught Exception: " << exc.what() << std::endl;
				doPrintError( err.str() );
				throw TestFailed( conditionName, file, function, line );
			}
			catch ( ... )
			{
				throw TestFailed( conditionName, file, function, line );
			}

			return result;
		}

		template< typename Type >
		bool checkThrow( Lazy< Type > const & condition
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				condition();
				reportFailure();
				std::stringstream err;
				err << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << std::endl;
				doPrintError( err.str() );
			}
			catch ( ... )
			{
				result = true;
			}

			return result;
		}

		template< typename Type >
		bool checkNoThrow( Lazy< Type > const & condition
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const conditionName )
		{
			addTest();
			bool result = false;

			try
			{
				condition();
				result = true;
			}
			catch ( std::exception & exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << file << " - " << function << ", line " << line << ": " << conditionName << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		bool checkEqual( ComparatorType compare
			, Lazy< LhsType > const & lhs
			, Lazy< RhsType > const & rhs
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const lhsName
			, char const * const rhsName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const & lhsRes = lhs();
				auto const & rhsRes = rhs();
				result = compare( lhsRes, rhsRes );

				if ( !result )
				{
					reportFailure();
					std::stringstream err;
					err << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " == " << rhsName << " (" << ::Testing::toString( lhsRes ) << " != " << ::Testing::toString( rhsRes ) << ")" << std::endl;
					doPrintError( err.str() );
				}
			}
			catch ( std::exception & exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " == " << rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " == " << rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename LhsType
			, typename RhsType
			, typename ComparatorType >
		bool checkNotEqual( ComparatorType compare
			, Lazy< LhsType > const & lhs
			, Lazy< RhsType > const & rhs
			, char const * const file
			, char const * const function
			, uint32_t const line
			, char const * const lhsName
			, char const * const rhsName )
		{
			addTest();
			bool result = false;

			try
			{
				auto const & lhsRes = lhs();
				auto const & rhsRes = rhs();
				result = compare( lhsRes, rhsRes );

				if ( result )
				{
					reportFailure();
					std::stringstream err;
					err << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " != " << rhsName << " (" << ::Testing::toString( lhsRes ) << " == " << ::Testing::toString( rhsRes ) << ")" << std::endl;
					doPrintError( err.str() );
				}
			}
			catch ( std::exception & exc )
			{
				std::stringstream err1;
				err1 << "Uncaught Exception: " << exc.what() << std::endl;
				doPrintError( err1.str() );
				reportFailure();
				std::stringstream err2;
				err2 << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " != " << rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err2.str() );
			}
			catch ( ... )
			{
				reportFailure();
				std::stringstream err;
				err << "Failure at " << file << " - " << function << ", line " << line << ": " << lhsName << " != " << rhsName << "(Unexpected exception)" << std::endl;
				doPrintError( err.str() );
			}

			return result;
		}

		template< typename T, typename U >
		bool compare( T const & lhs, U const & rhs )
		{
			return lhs == rhs;
		}

		template< typename T >
		bool compare( std::pair< T const *, uint32_t > const & lhs, std::pair< T const *, uint32_t > rhs )
		{
			bool result = lhs.second == rhs.second;

			if ( result )
			{
				result = std::memcmp( lhs.first, rhs.first, lhs.second * sizeof( T ) ) == 0;
			}

			return result;
		}

		template< typename T >
		bool compare( std::pair< T *, uint32_t > const & lhs, std::pair< T *, uint32_t > rhs )
		{
			bool result = lhs.second == rhs.second;

			if ( result )
			{
				result = std::memcmp( lhs.first, rhs.first, lhs.second * sizeof( T ) ) == 0;
			}

			return result;
		}

		template< typename Key, typename Value >
		bool compare( std::map< Key, Value > const & lhs, std::map< Key, Value > rhs )
		{
			bool result = lhs.size() == rhs.size();

			for ( auto & itA : lhs )
			{
				if ( result )
				{
					auto itB = rhs.find( itA.first );
					result = itB != rhs.end();
					result &= compare( itA.second, itB->second );
				}
			}

			return result;
		}

		template< typename Key, typename Value >
		bool compare( std::map< Key, std::shared_ptr< Value > > const & lhs, std::map< Key, std::shared_ptr< Value > > rhs )
		{
			bool result = lhs.size() == rhs.size();

			for ( auto & itA : lhs )
			{
				if ( result )
				{
					auto itB = rhs.find( itA.first );
					result = itB != rhs.end();
					result &= compare( *itA.second, *itB->second );
				}
			}

			return result;
		}

		bool compare( float const & lhs, float const & rhs )
		{
			float epsilon = 0.0001f;
			return std::abs( lhs - rhs ) < epsilon
				   || ( std::isnan( lhs ) && std::isnan( rhs ) );
		}

		bool compare( float const & lhs, double const & rhs )
		{
			float epsilon = 0.0001f;
			return std::abs( lhs - rhs ) < epsilon
				   || ( std::isnan( lhs ) && std::isnan( rhs ) );
		}

		bool compare( double const & lhs, double const & rhs )
		{
			double epsilon = 0.0001;
			return std::abs( lhs - rhs ) < epsilon
				   || ( std::isnan( lhs ) && std::isnan( rhs ) );
		}

		bool compare( double const & lhs, float const & rhs )
		{
			double epsilon = 0.0001;
			return std::abs( lhs - rhs ) < epsilon
				|| ( std::isnan( lhs ) && std::isnan( rhs ) );
		}

		template< typename T, size_t C1, size_t C2 >
		bool compare( std::array< T, C1 > const & lhs, std::array< T, C2 > const & rhs )
		{
			bool result{ C1 == C2 };

			for ( size_t i = 0u; i < C1 && result; ++i )
			{
				result = this->compare( lhs[i], rhs[i] );
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

#	define CT_NAME_CONCAT_( X, Y ) X ## Y
#	define CT_NAME_CONCAT( X, Y ) CT_NAME_CONCAT_( X, Y )

#	define CT_FAILURE_EX( test, x )\
	( test ).fail( __FILE__, __FUNCTION__, uint32_t( __LINE__ ), x )

#	define CT_CHECK_EX( test, x )\
	( test ).check( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_EQUAL_EX( test, x, y )\
	( test ).checkEqual( [&]( auto const & plhs, auto const & prhs ){ return ( test ).compare( plhs, prhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x, #y )

#	define CT_NEQUAL_EX( test, x, y )\
	( test ).checkNotEqual( [&]( auto const & plhs, auto const & prhs ){ return ( test ).compare( plhs, prhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x, #y )

#	define CT_CHECK_THROW_EX( test, x )\
	( test ).checkThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_CHECK_NOTHROW_EX( test, x )\
	( test ).checkNoThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_REQUIRE_EX( test, x )\
	( test ).require( LAZY( ( x ) ), __FILE__, __FUNCTION__, uint32_t( __LINE__ ), #x )

#	define CT_ON_EX( test, text )\
	Testing::TestBlockPtr CT_NAME_CONCAT( onBlock, __LINE__ ){ ( test ).on( text ) }

#	define CT_WHEN_EX( test, text )\
	Testing::TestBlockPtr CT_NAME_CONCAT( whenBlock, __LINE__ ){ ( test ).when( text ) }

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
