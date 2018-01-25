#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlImageAspectFlags convert( renderer::ImageAspectFlags const & flags )
	{
		GLenum result{ 0 };

		if ( checkFlag( flags, renderer::ImageAspectFlag::eColour ) )
		{
			result |= GL_COLOR_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eDepth ) )
		{
			result |= GL_DEPTH_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eStencil ) )
		{
			result |= GL_STENCIL_BUFFER_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eMetaData ) )
		{
		}

		return result;
	}
}
