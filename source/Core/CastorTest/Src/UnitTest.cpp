#include "UnitTest.hpp"

namespace Testing
{
	//*************************************************************************************************

	namespace
	{
		std::string Center( uint32_t p_width, std::string const & p_text )
		{
			size_t l_length = p_text.size();
			std::stringstream l_stream;
			l_stream.width( ( p_width - l_length ) / 2 );
			l_stream << " ";
			return l_stream.str() + p_text;
		}
	}

	//*************************************************************************************************

	TestCase::TestCase( std::string const & p_name )
		: m_name( p_name )
	{
	}

	TestCase::~TestCase()
	{
	}

	void TestCase::RegisterTests()
	{
		DoRegisterTests();
	}

	void TestCase::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		m_errorCount = &p_errCount;
		m_testCount = &p_testCount;

		for ( auto l_test : m_tests )
		{
			std::stringstream l_begin;
			l_begin << "**** ";
			l_begin.width( TEST_TITLE_WIDTH - 10 );
			l_begin << std::left << ( "Begin test case " + l_test.first ) << " ****";
			std::cout << l_begin.str() << std::endl;

			try
			{
				l_test.second();
			}
			catch ( TestFailed & exc )
			{
				p_errCount++;
				std::cerr << "*	Test " << l_test.first << " failed (" << exc.what() << ")" << std::endl;
			}
			catch ( std::exception & exc )
			{
				p_errCount++;
				std::cerr << "*	Test " << l_test.first << " execution failed (" << exc.what() << ")" << std::endl;
			}
			catch ( ... )
			{
				p_errCount++;
				std::cerr << "*	Test " << l_test.first << " execution failed (Unknown reason)" << std::endl;
			}

			std::stringstream l_end;
			l_end << "**** ";
			l_end.width( TEST_TITLE_WIDTH - 10 );
			l_end << std::left << ( "End test case " + l_test.first ) << " ****";
			std::cout << l_end.str() << std::endl;
		}
	}

	void TestCase::DoRegisterTest( std::string const & p_name, TestFunction p_test )
	{
		m_tests.push_back( { p_name, p_test } );
	}

	//*************************************************************************************************

	TestFailed::TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line )
		: m_what( p_file + " - " + p_function + ", line " + std::to_string( p_line ) + " : " + p_what )
	{
	}

	TestFailed::~TestFailed() throw()
	{
	}

	//*************************************************************************************************
}
