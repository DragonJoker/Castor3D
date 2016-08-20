#include "State/TestMultisampleState.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

namespace TestRender
{
	TestMultisampleState::TestMultisampleState( TestRenderSystem * p_renderSystem )
		: MultisampleState( *p_renderSystem->GetEngine() )
		, m_renderSystem( p_renderSystem )
	{
	}

	TestMultisampleState::~TestMultisampleState()
	{
	}

	bool TestMultisampleState::Apply()const
	{
		return true;
	}
}
