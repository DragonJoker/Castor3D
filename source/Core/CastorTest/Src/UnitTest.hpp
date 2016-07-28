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

#include "TestPrerequisites.hpp"

#include <cmath>

namespace Testing
{
	template< typename T >
	inline std::string to_string (T const & p_value)
	{
		std::stringstream l_stream;
		l_stream << p_value;
		return l_stream.str ();
	}

	template<>
	inline std::string to_string< std::wstring > (std::wstring const & p_value)
	{
		std::stringstream l_stream;
		l_stream << Castor::string::string_cast<char>(p_value);
		return l_stream.str ();
	}

	template< typename T >
	inline std::string to_string (std::pair< T const *, uint32_t > const & p_value)
	{
		std::stringstream l_stream;
		l_stream << p_value.second << ": ";

		std::for_each (p_value.first, p_value.first + p_value.second, [&l_stream] (T const & p_val)
		{
			l_stream << " " << p_val;
		});

		return l_stream.str ();
	}

	template< class Value >
	class Lazy
	{
		typedef std::function< Value () > Getter;

	public:
		Lazy (std::function< Value () > const & p_expression)
			: m_thunk {std::make_shared< Getter > ([p_expression] () -> Value
			{
				return p_expression ();
			})}
		{
		}

			Lazy (Lazy const &) = delete;

			const Value & operator()()const
			{
				return (*m_thunk)();
			}

			Value & operator()()
			{
				return (*m_thunk)();
			}

	private:
		mutable std::shared_ptr< Getter > m_thunk;
	};

	template< class Value >
	class Lazy< Value & >
	{
		typedef std::function< Value &() > Getter;

	public:
		Lazy (std::function< Value &() > const & p_expression)
			: m_thunk {std::make_shared< Getter > ([p_expression] () -> Value &
			{
				return p_expression ();
			})}
		{
		}

			Lazy (Lazy const &) = delete;

			const Value & operator()()const
			{
				return (*m_thunk)();
			}

			Value & operator()()
			{
				return (*m_thunk)();
			}

	private:
		mutable std::shared_ptr< Getter > m_thunk;
	};

	class TestCase
	{
	public:
		using TestFunction = std::function< void () >;

	public:
		explicit TestCase (std::string const & p_name);
		virtual ~TestCase ();
		void RegisterTests ();
		void Execute (uint32_t & p_errCount, uint32_t & p_testCount);

		inline std::string const & GetName ()const
		{
			return m_name;
		}

		inline void ReportFailure ()
		{
			(*m_errorCount)++;
		}

		inline void AddTest ()
		{
			(*m_testCount)++;
		}

	protected:
		void DoRegisterTest (std::string const & p_name, TestFunction p_test);

	private:
		virtual void DoRegisterTests () = 0;

	protected:
		template< typename Type >
		inline bool Check (Lazy< Type > const & p_condition, char const * const p_file, char const * const p_function, uint32_t const p_line, char const * const p_conditionName)
		{
			AddTest ();
			bool l_return = false;

			try
			{
				auto const l_condition = p_condition ();

				if (!l_condition)
				{
					ReportFailure ();
					Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName);
				}
				else
				{
					l_return = true;
				}
			}
			catch (...)
			{
				ReportFailure ();
				Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName << "(Unexpected exception)");
			}

			return l_return;
		}

		template< typename Type >
		inline bool Require (Lazy< Type > const & p_condition, char const * const p_file, char const * const p_function, uint32_t const p_line, char const * const p_conditionName)
		{
			AddTest ();
			bool l_return = false;

			try
			{
				auto const l_condition = p_condition ();

				if (!l_condition)
				{
					throw TestFailed (p_conditionName, p_file, p_function, p_line);
				}
			}
			catch (...)
			{
				throw TestFailed (p_conditionName, p_file, p_function, p_line);
			}

			return l_return;
		}

		template< typename Type >
		inline bool CheckThrow (Lazy< Type > const & p_condition, char const * const p_file, char const * const p_function, uint32_t const p_line, char const * const p_conditionName)
		{
			AddTest ();
			bool l_return = false;

			try
			{
				p_condition ();
				ReportFailure ();
				Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName);
			}
			catch (...)
			{
				l_return = true;
			}

			return l_return;
		}

		template< typename Type >
		inline bool CheckNoThrow (Lazy< Type > const & p_condition, char const * const p_file, char const * const p_function, uint32_t const p_line, char const * const p_conditionName)
		{
			AddTest ();
			bool l_return = false;

			try
			{
				p_condition ();
				l_return = true;
			}
			catch (...)
			{
				ReportFailure ();
				Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_conditionName);
			}

			return l_return;
		}

		template< typename LhsType, typename RhsType, typename ComparatorType >
		inline bool CheckEqual( ComparatorType p_compare, Lazy< LhsType > const & p_lhs, Lazy< RhsType > const & p_rhs, char const * const p_file, char const * const p_function, uint32_t const p_line, char const * const p_lhsName, char const * const p_rhsName )
		{
			AddTest ();
			bool l_return = false;

			try
			{
				auto const & l_lhs = p_lhs ();
				auto const & l_rhs = p_rhs ();
				l_return = p_compare( l_lhs, l_rhs );

				if (!l_return)
				{
					ReportFailure ();
					Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << " (" << ::Testing::to_string (l_lhs) << " != " << ::Testing::to_string (l_rhs) << ")");
				}
			}
			catch (...)
			{
				ReportFailure ();
				Castor::Logger::LogWarning (std::stringstream () << "Failure at " << p_file << " - " << p_function << ", line " << p_line << ": " << p_lhsName << " == " << p_rhsName << "(Unexpected exception)");
			}

			return l_return;
		}

		template< typename T, typename U >
		inline bool compare( T const & p_lhs, U const & p_rhs )
		{
			return p_lhs == p_rhs;
		}

		template< typename T >
		inline bool compare (std::pair< T const *, uint32_t > const & p_a, std::pair< T const *, uint32_t > p_b)
		{
			bool l_return = p_a.second == p_b.second;

			if (l_return)
			{
				l_return = std::memcmp (p_a.first, p_b.first, p_a.second * sizeof (T)) == 0;
			}

			return l_return;
		}

		inline bool compare (float const & p_a, float const & p_b)
		{
			float l_epsilon = float (0.0001);
			return std::abs (float (p_a - p_b)) < l_epsilon
				|| (std::isnan (p_a) && std::isnan (p_b));
		}

		inline bool compare (float const & p_a, double const & p_b)
		{
			float l_epsilon = float (0.0001);
			return std::abs (float (p_a - p_b)) < l_epsilon
				|| (std::isnan (p_a) && std::isnan (p_b));
		}

		inline bool compare (double const & p_a, double const & p_b)
		{
			double l_epsilon = double (0.0001);
			return std::abs (double (p_a - p_b)) < l_epsilon
				|| (std::isnan (p_a) && std::isnan (p_b));
		}

		inline bool compare (double const & p_a, float const & p_b)
		{
			double l_epsilon = double (0.0001);
			return std::abs (double (p_a - p_b)) < l_epsilon
				|| (std::isnan (p_a) && std::isnan (p_b));
		}

	private:
		uint32_t * m_errorCount {nullptr};
		uint32_t * m_testCount {nullptr};
		std::string m_name;
		std::vector< std::pair< std::string, TestFunction > > m_tests;
	};

	DECLARE_SMART_PTR (TestCase);

	class TestFailed
		: public std::exception
	{
	public:
		TestFailed (std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line);
		virtual ~TestFailed () throw();
		const char * what ()
		{
			return m_what.c_str ();
		}

	private:
		std::string m_what;
	};

#	define LAZY( E ) Lazy< decltype( ( E ) ) >( std::function< decltype( ( E ) )() >( [&]() -> decltype( ( E ) )\
	{\
		return E;\
	} ) )

#	define CT_CHECK( x )\
	Check( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_EQUAL( x, y )\
	CheckEqual( [&]( auto const & lhs, auto const & rhs ){ return compare( lhs, rhs ); }, LAZY( ( x ) ), LAZY( ( y ) ), __FILE__, __FUNCTION__, __LINE__, #x, #y )

#	define CT_CHECK_THROW( x )\
	CheckThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_CHECK_NOTHROW( x )\
	CheckNoThrow( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )

#	define CT_REQUIRE( x )\
	Require( LAZY( ( x ) ), __FILE__, __FUNCTION__, __LINE__, #x )
}

#endif
