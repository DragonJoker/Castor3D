#include "UnitTest.hpp"

using namespace Castor;

namespace Testing
{
	TestFailed::TestFailed( std::string const & p_what, std::string const & p_file, std::string const & p_function, int p_line )
		: m_what( p_file + " - " + p_function + ", line " + std::to_string( p_line ) + " : " + p_what )
	{
	}

	TestFailed::~TestFailed() throw()
	{
	}

	//*************************************************************************************************

	TestCase::TestCase( std::string const & p_strName )
		:	m_strName( p_strName	)
	{
	}

	TestCase::~TestCase()
	{
	}
}
