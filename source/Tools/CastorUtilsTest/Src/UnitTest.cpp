#include "UnitTest.hpp"

using namespace Castor;

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

	TestFailed::TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line )
		: m_what( p_file + " - " + p_function + ", line " + std::to_string( p_line ) + " : " + p_what )
	{
	}

	TestFailed::~TestFailed() throw()
	{
	}

	//*************************************************************************************************

	TestCase::TestCase( std::string const & p_strName )
		: m_strName( p_strName )
	{
	}

	TestCase::~TestCase()
	{
	}

	void TestCase::DoExecuteTest( std::function< void( uint32_t &, uint32_t & ) > p_test, uint32_t & p_errCount, uint32_t & p_testCount, std::string const & p_name )
	{
		std::stringstream l_begin;
		l_begin << "**** ";
		l_begin.width( TEST_TITLE_WIDTH - 10 );
		l_begin << std::left << ( "Begin test case " + p_name ) << " ****";
		Logger::LogInfo( l_begin );
		p_test( p_errCount, p_testCount );
		std::stringstream l_end;
		l_end << "**** ";
		l_end.width( TEST_TITLE_WIDTH - 10 );
		l_end << std::left << ( "End test case " + p_name ) << " ****";
		Logger::LogInfo( l_end );
	}
}
