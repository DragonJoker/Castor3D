#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GLFilter convert( renderer::Filter const & filter )
	{
		switch ( filter )
		{
		case renderer::Filter::eNearest:
			return GL_FILTER_NEAREST;

		case renderer::Filter::eLinear:
			return GL_FILTER_LINEAR;

		default:
			assert( false );
			return GL_FILTER_NEAREST;
		}
	}

	GLFilter convert( renderer::Filter const & filter, renderer::MipmapMode mode )
	{
		switch ( filter )
		{
		case renderer::Filter::eNearest:
			switch ( mode )
			{
			case renderer::MipmapMode::eNone:
				return GL_FILTER_NEAREST;

			case renderer::MipmapMode::eNearest:
				return GL_FILTER_NEAREST_MIPMAP_NEAREST;

			case renderer::MipmapMode::eLinear:
				return GL_FILTER_NEAREST_MIPMAP_LINEAR;

			default:
				assert( false );
				return GL_FILTER_NEAREST;
			}

		case renderer::Filter::eLinear:
			switch ( mode )
			{
			case renderer::MipmapMode::eNone:
				return GL_FILTER_LINEAR;

			case renderer::MipmapMode::eNearest:
				return GL_FILTER_LINEAR_MIPMAP_NEAREST;

			case renderer::MipmapMode::eLinear:
				return GL_FILTER_LINEAR_MIPMAP_LINEAR;

			default:
				assert( false );
				return GL_FILTER_LINEAR;
			}

		default:
			assert( false );
			return GL_FILTER_NEAREST;
		}
	}
}
