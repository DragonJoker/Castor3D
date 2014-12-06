#include "SamplerRenderer.hpp"

using namespace Castor;

namespace Castor3D
{
	SamplerRenderer::SamplerRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer< Sampler, SamplerRenderer >( p_pRenderSystem )
	{
	}

	SamplerRenderer::~SamplerRenderer()
	{
	}
}
