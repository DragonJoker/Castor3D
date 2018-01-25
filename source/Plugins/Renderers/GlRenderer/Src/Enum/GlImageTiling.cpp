#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlImageTiling convert( renderer::ImageTiling const & value )
	{
		switch ( value )
		{
		case renderer::ImageTiling::eOptimal:
			return GL_OPTIMAL_TILING_EXT;

		case renderer::ImageTiling::eLinear:
			return GL_LINEAR_TILING_EXT;

		default:
			assert( false && "Unsupported image tiling" );
			return GL_OPTIMAL_TILING_EXT;
		}
	}
}
