#include "UnitTest.hpp"

#include <sstream>

namespace Testing
{
	//*************************************************************************************************

	namespace
	{
		std::string Center( uint32_t p_width, std::string const & p_text )
		{
			size_t length = p_text.size();
			std::stringstream stream;
			stream.width( ( p_width - length ) / 2 );
			stream << " ";
			return stream.str() + p_text;
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

	void TestCase::DoRegisterTest( std::string const & p_name, TestFunction p_test )
	{
		m_tests.push_back( { p_name, p_test } );
	}

	//*************************************************************************************************

	std::string to_string( int p_value )
	{
		std::stringstream stream;
		stream << p_value;
		return stream.str();
	}

	TestFailed::TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line )
		: m_what( p_file + " - " + p_function + ", line " + to_string( p_line ) + " : " + p_what )
	{
	}

	TestFailed::~TestFailed() throw()
	{
	}

	//*************************************************************************************************
}
