#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlCullMode convert( renderer::CullModeFlags const & flags )
	{
		GlCullMode result{ GL_CULL_MODE_NONE };

		if ( checkFlag( flags, renderer::CullModeFlag::eFrontAndBack ) )
		{
			result = GL_CULL_MODE_FRONT_AND_BACK;
		}
		else if ( checkFlag( flags, renderer::CullModeFlag::eFront ) )
		{
			result = GL_CULL_MODE_FRONT;
		}
		else if ( checkFlag( flags, renderer::CullModeFlag::eBack ) )
		{
			result = GL_CULL_MODE_BACK;
		}

		return result;
	}
}
