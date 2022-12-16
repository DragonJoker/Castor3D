#include "UnitTest.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <sstream>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

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

	TestCase::TestCase( std::string const & name )
		: m_name( name )
	{
	}

	TestCase::~TestCase()
	{
	}

	void TestCase::registerTests()
	{
		doRegisterTests();
	}

	void TestCase::execute( uint32_t & errCount, uint32_t & testCount )
	{
		m_errorCount = &errCount;
		m_testCount = &testCount;

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
				errCount++;
				std::cerr << "*	Test " << test.first << " failed (" << exc.what() << ")" << std::endl;
			}
			catch ( std::exception & exc )
			{
				errCount++;
				std::cerr << "*	Test " << test.first << " execution failed (" << exc.what() << ")" << std::endl;
			}
			catch ( ... )
			{
				errCount++;
				std::cerr << "*	Test " << test.first << " execution failed (Unknown reason)" << std::endl;
			}

			std::stringstream end;
			end << "**** ";
			end.width( TEST_TITLE_WIDTH - 10 );
			end << std::left << ( "End test case " + test.first ) << " ****";
			std::cout << end.str() << std::endl;
		}
	}

	void TestCase::doRegisterTest( std::string const & name
		, TestFunction test )
	{
		m_tests.push_back( { name, test } );
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

	TestFailed::TestFailed( std::string const & what
		, std::string const & file
		, std::string const & function
		, uint32_t line )
		: m_what( file + " - " + function + ", line " + toString( line ) + " : " + what )
	{
	}

	//*************************************************************************************************
}
