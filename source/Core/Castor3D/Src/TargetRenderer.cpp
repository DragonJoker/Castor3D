#include "TargetRenderer.hpp"
#include "RenderTarget.hpp"

using namespace Castor;

namespace Castor3D
{
	TargetRenderer::TargetRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer< RenderTarget, TargetRenderer >( p_pRenderSystem )
	{
	}

	TargetRenderer::~TargetRenderer()
	{
	}
}
