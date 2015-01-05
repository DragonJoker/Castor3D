#include "LightRenderer.hpp"
#include "Light.hpp"

using namespace Castor;

namespace Castor3D
{
	LightRenderer::LightRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer<Light, LightRenderer>( p_pRenderSystem )
	{
	}

	LightRenderer::~LightRenderer()
	{
		Cleanup();
	}

	void LightRenderer::Initialise()
	{
	}

	void LightRenderer::Cleanup()
	{
	}
}
