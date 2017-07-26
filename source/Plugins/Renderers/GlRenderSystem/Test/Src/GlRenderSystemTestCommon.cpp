#include "GlRenderSystemTestCommon.hpp"

#include <Engine.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Testing
{
	GlTestCase::GlTestCase( std::string const & p_name, Castor3D::Engine & engine )
		: TestCase{ p_name }
		, m_engine{ engine }
	{
	}

	//*********************************************************************************************
}
