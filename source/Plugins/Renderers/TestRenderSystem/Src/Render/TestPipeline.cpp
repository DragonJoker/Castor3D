#include "Render/TestPipeline.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestPipeline::TestPipeline( TestRenderSystem & p_renderSystem
								, DepthStencilState && p_dsState
								, RasteriserState && p_rsState
								, BlendState && p_bdState
								, MultisampleState && p_msState
								, ShaderProgram & p_program )
		: Pipeline{ p_renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program }
	{
	}

	TestPipeline::~TestPipeline()
	{
	}

	void TestPipeline::Apply()const
	{
	}

	void TestPipeline::ApplyViewport( int p_windowWidth, int p_windowHeight )
	{
	}
}
