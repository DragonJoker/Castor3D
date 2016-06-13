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
		CreateCurrent();
	}

	TestRasteriserState::~TestRasteriserState()
	{
		DestroyCurrent();
	}

	bool TestRasteriserState::Apply()
	{
		m_changed = false;
		return true;
	}

	RasteriserStateSPtr TestRasteriserState::DoCreateCurrent()
	{
		return std::make_shared< TestRasteriserState >( m_renderSystem );
	}
}
