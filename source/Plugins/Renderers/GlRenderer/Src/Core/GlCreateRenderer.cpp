#include "Core/GlCreateRenderer.hpp"
#include "Core/GlRenderer.hpp"

extern "C"
{
	GlRenderer_API renderer::Renderer * createRenderer( renderer::Renderer::Configuration const & configuration )
	{
		return new gl_renderer::Renderer{ configuration };
	}
}
