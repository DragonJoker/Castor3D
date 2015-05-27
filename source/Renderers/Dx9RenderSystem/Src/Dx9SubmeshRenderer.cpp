#include "Dx9SubmeshRenderer.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor3D;

namespace Dx9Render
{
	DxSubmeshRenderer::DxSubmeshRenderer( DxRenderSystem * p_pRenderSystem )
		:	SubmeshRenderer( p_pRenderSystem )
	{
	}

	DxSubmeshRenderer::~DxSubmeshRenderer()
	{
	}
}
