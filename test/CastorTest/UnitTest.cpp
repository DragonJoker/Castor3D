#include "UnitTest.hpp"

#include <sstream>

namespace Testing
{
	//*************************************************************************************************

	TestBlock::TestBlock( TestCase & testCase
		, std::string const & text )
		: testCase{ testCase }
		, text{ text }
	{
	}

	TestBlock::~TestBlock()
	{
		testCase.doPopBlock( this );
	}

	//*************************************************************************************************

	TestCase::TestCase( std::string const & p_name )
		: m_name( p_name )
	{
	}

	TestCase::~TestCase()
	{
	}

	void TestCase::registerTests()
	{
		doRegisterTests();
	}

	void TestCase::execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		m_errorCount = &p_errCount;
		m_testCount = &p_testCount;

		for ( auto test : m_tests )
		{
			std::stringstream begin;
			begin << "**** ";
			begin.width( TEST_TITLE_WIDTH - 10 );
			begin << std::left << ( "Begin test case " + test.first ) << " ****";
			std::cout << begin.str() << std::endl;

			try
			{
				test.second();
			}
			catch ( TestFailed & exc )
			{
				p_errCount++;
				std::cerr << "*	Test " << test.first << " failed (" << exc.what() << ")" << std::endl;
			}
			catch ( std::exception & exc )
			{
				p_errCount++;
				std::cerr << "*	Test " << test.first << " execution failed (" << exc.what() << ")" << std::endl;
			}
			catch ( ... )
			{
				p_errCount++;
				std::cerr << "*	Test " << test.first << " execution failed (Unknown reason)" << std::endl;
			}

			std::stringstream end;
			end << "**** ";
			end.width( TEST_TITLE_WIDTH - 10 );
			end << std::left << ( "End test case " + test.first ) << " ****";
			std::cout << end.str() << std::endl;
		}
	}

	void TestCase::doRegisterTest( std::string const & p_name, TestFunction p_test )
	{
		m_tests.push_back( { p_name, p_test } );
	}

	TestBlockPtr TestCase::doPushBlock( std::string const & text )
	{
		auto result = std::make_unique< TestBlock >( *this, text );
		m_blocks.push_back( result.get() );
		return result;
	}

	void TestCase::doPopBlock( TestBlock * block )
	{
		auto it = std::find( m_blocks.begin(), m_blocks.end(), block );
		m_blocks.erase( it );
	}

	void TestCase::doPrintError( std::string const & error )
	{
		std::string prefix;

		for ( auto block : m_blocks )
		{
			std::cerr << prefix << block->text << std::endl;
			prefix += "  ";
		}

		std::cerr << prefix << error << std::endl;
	}

	//*************************************************************************************************

	std::string toString( int p_value )
	{
		std::stringstream stream;
		stream << p_value;
		return stream.str();
	}

	TestFailed::TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line )
		: m_what( p_file + " - " + p_function + ", line " + toString( p_line ) + " : " + p_what )
	{
	}

	TestFailed::~TestFailed() throw()
	{
	}

	//*************************************************************************************************
}
