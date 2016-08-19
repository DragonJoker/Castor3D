#include "State/TestRasteriserState.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestRasteriserState::TestRasteriserState( TestRenderSystem * p_renderSystem )
		: RasteriserState( *p_renderSystem->GetEngine() )
		, m_renderSystem( p_renderSystem )
	{
	}

	TestRasteriserState::~TestRasteriserState()
	{
	}

	bool TestRasteriserState::Apply()const
	{
		return true;
	}
}
