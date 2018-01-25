#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlMemoryPropertyFlags convert( renderer::MemoryPropertyFlags const & flags )
	{
		GlMemoryPropertyFlags result{ 0 };

		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostVisible ) )
		{
			result = GL_MEMORY_PROPERTY_READ_BIT
				| GL_MEMORY_PROPERTY_WRITE_BIT
				| GL_MEMORY_PROPERTY_DYNAMIC_STORAGE_BIT;
		}

		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostCoherent ) )
		{
			result = GL_MEMORY_PROPERTY_COHERENT_BIT
				| GL_MEMORY_PROPERTY_PERSISTENT_BIT;
		}

		return result;
	}
}
