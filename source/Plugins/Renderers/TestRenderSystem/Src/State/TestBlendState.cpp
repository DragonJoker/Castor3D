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
		CreateCurrent();
	}

	TestBlendState::~TestBlendState()
	{
		DestroyCurrent();
	}

	bool TestBlendState::Apply()
	{
		m_changed = false;
		return true;
	}

	BlendStateSPtr TestBlendState::DoCreateCurrent()
	{
		return std::make_shared< TestBlendState >( m_renderSystem );
	}
}
