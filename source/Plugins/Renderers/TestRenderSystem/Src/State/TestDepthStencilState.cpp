#include "State/TestDepthStencilState.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestDepthStencilState::TestDepthStencilState( TestRenderSystem * p_renderSystem )
		: DepthStencilState( *p_renderSystem->GetEngine() )
		, m_renderSystem( p_renderSystem )
	{
	}

	TestDepthStencilState::~TestDepthStencilState()
	{
	}

	bool TestDepthStencilState::Apply()const
	{
		return true;
	}
}
