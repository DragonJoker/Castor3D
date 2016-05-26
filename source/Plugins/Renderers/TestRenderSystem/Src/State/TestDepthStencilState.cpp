#include "State/TestDepthStencilState.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestDepthStencilState::TestDepthStencilState( TestRenderSystem * p_renderSystem )
		: DepthStencilState( *p_renderSystem->GetEngine() )
	{
		CreateCurrent();
	}

	TestDepthStencilState::~TestDepthStencilState()
	{
		DestroyCurrent();
	}

	bool TestDepthStencilState::Apply()
	{
		m_changed = false;
		return true;
	}

	DepthStencilStateSPtr TestDepthStencilState::DoCreateCurrent()
	{
		return std::make_shared< TestDepthStencilState >( m_renderSystem );
	}
}
