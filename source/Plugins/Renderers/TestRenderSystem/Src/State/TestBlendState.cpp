#include "State/TestBlendState.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestBlendState::TestBlendState( TestRenderSystem * p_renderSystem )
		: BlendState( *p_renderSystem->GetEngine() )
		, m_renderSystem( p_renderSystem )
	{
	}

	TestBlendState::~TestBlendState()
	{
	}

	bool TestBlendState::Apply()const
	{
		return true;
	}
}
