#include "GlRenderSystemTestCommon.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace Testing
{
	GlTestCase::GlTestCase( std::string const & p_name, castor3d::Engine & engine )
		: TestCase{ p_name }
		, m_engine{ engine }
	{
	}

	//*********************************************************************************************
}
