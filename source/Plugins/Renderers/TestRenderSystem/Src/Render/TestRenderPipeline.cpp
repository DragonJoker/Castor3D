#include "Render/TestRenderPipeline.hpp"

#include "Render/TestRenderSystem.hpp"

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestRenderPipeline::TestRenderPipeline(
		TestRenderSystem & p_renderSystem,
		DepthStencilState && p_dsState,
		RasteriserState && p_rsState,
		BlendState && p_bdState,
		MultisampleState && p_msState,
		ShaderProgram & p_program,
		PipelineFlags const & p_flags )
		: RenderPipeline{ p_renderSystem, std::move( p_dsState ), std::move( p_rsState ), std::move( p_bdState ), std::move( p_msState ), p_program, p_flags }
	{
	}

	TestRenderPipeline::~TestRenderPipeline()
	{
	}

	void TestRenderPipeline::apply()const
	{
	}
}
