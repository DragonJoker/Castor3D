#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlFilter value )
	{
		switch ( value )
		{
		case gl_renderer::GL_FILTER_NEAREST:
			return "GL_NEAREST";

		case gl_renderer::GL_FILTER_LINEAR:
			return "GL_LINEAR";

		case gl_renderer::GL_FILTER_NEAREST_MIPMAP_NEAREST:
			return "GL_NEAREST_MIPMAP_NEAREST";

		case gl_renderer::GL_FILTER_LINEAR_MIPMAP_NEAREST:
			return "GL_LINEAR_MIPMAP_NEAREST";

		case gl_renderer::GL_FILTER_NEAREST_MIPMAP_LINEAR:
			return "GL_NEAREST_MIPMAP_LINEAR";

		case gl_renderer::GL_FILTER_LINEAR_MIPMAP_LINEAR:
			return "GL_LINEAR_MIPMAP_LINEAR";

		default:
			assert( false && "Unsupported GlFilter" );
			return "GLFilter_UNKNOWN";
		}
	}

	GlFilter convert( renderer::Filter const & filter )
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

	GlFilter convert( renderer::Filter const & filter, renderer::MipmapMode mode )
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
