#include "Dx10TextureRenderer.hpp"
#include "Dx10RenderSystem.hpp"
#include <Pixel.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx10Render
{
	DxTextureRenderer::DxTextureRenderer( DxRenderSystem * p_pRenderSystem )
		:	TextureRenderer( p_pRenderSystem	)
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
