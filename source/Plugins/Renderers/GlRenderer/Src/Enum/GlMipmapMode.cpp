#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GLenum convert( renderer::MipmapMode const & mode )
	{
		switch ( mode )
		{
		case renderer::MipmapMode::eNearest:
			return GL_FILTER_NEAREST_MIPMAP_NEAREST;

		case renderer::MipmapMode::eLinear:
			return GL_FILTER_LINEAR_MIPMAP_LINEAR;

		default:
			assert( false );
			return GL_FILTER_NEAREST_MIPMAP_NEAREST;
		}
	}
}
