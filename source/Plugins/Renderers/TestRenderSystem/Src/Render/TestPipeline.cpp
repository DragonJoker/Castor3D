#include "Render/TestPipeline.hpp"

#include "Render/TestRenderSystem.hpp"
#include "State/TestRasteriserState.hpp"
#include "State/TestBlendState.hpp"
#include "State/TestMultisampleState.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestPipeline::TestPipeline( TestRenderSystem & p_renderSystem
								, RasteriserStateUPtr && p_rsState
								, BlendStateUPtr && p_bdState
								, MultisampleStateUPtr && p_msState )
		: Pipeline{ p_renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ) }
	{
	}

	TestPipeline::~TestPipeline()
	{
	}

	void TestPipeline::Apply()const
	{
		static_cast< TestRasteriserState & >( *m_rsState ).Apply();
		static_cast< TestBlendState & >( *m_blState ).Apply();
		static_cast< TestMultisampleState & >( *m_msState ).Apply();
	}

	void TestPipeline::ApplyViewport( int p_windowWidth, int p_windowHeight )
	{
	}
}
