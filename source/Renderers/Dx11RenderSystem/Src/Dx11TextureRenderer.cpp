#include "Dx11TextureRenderer.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx11Render
{
	DxTextureRenderer::DxTextureRenderer( DxRenderSystem * p_pRenderSystem )
		:	TextureRenderer( p_pRenderSystem )
	{
	}

	DxTextureRenderer::~DxTextureRenderer()
	{
	}

	bool DxTextureRenderer::Render()
	{
		return true;
	}

	void DxTextureRenderer::EndRender()
	{
	}
}

#pragma warning( pop)
