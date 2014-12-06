#include "TextureRenderer.hpp"

using namespace Castor;

namespace Castor3D
{
	TextureRenderer::TextureRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer< TextureUnit, TextureRenderer >( p_pRenderSystem )
	{
	}

	TextureRenderer::~TextureRenderer()
	{
	}
}
