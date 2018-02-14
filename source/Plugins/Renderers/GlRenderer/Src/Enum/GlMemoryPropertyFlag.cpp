#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlMemoryPropertyFlags value )
	{
		std::string result;
		std::string sep;

		if ( checkFlag( value, gl_renderer::GlMemoryPropertyFlag::GL_MEMORY_PROPERTY_READ_BIT ) )
		{
			result += sep + "GL_MAP_READ_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlMemoryPropertyFlag::GL_MEMORY_PROPERTY_WRITE_BIT ) )
		{
			result += sep + "GL_MAP_WRITE_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlMemoryPropertyFlag::GL_MEMORY_PROPERTY_DYNAMIC_STORAGE_BIT ) )
		{
			result += sep + "GL_DYNAMIC_STORAGE_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlMemoryPropertyFlag::GL_MEMORY_PROPERTY_COHERENT_BIT ) )
		{
			result += sep + "GL_MAP_COHERENT_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlMemoryPropertyFlag::GL_MEMORY_PROPERTY_PERSISTENT_BIT ) )
		{
			result += sep + "GL_MAP_INVALIDATE_RANGE_BIT";
		}

		return result;
	}

	GlMemoryPropertyFlags convert( renderer::MemoryPropertyFlags const & flags )
	{
		GlMemoryPropertyFlags result{ 0 };

		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostVisible ) )
		{
			result = GL_MEMORY_PROPERTY_READ_BIT
				| GL_MEMORY_PROPERTY_WRITE_BIT
				/*| GL_MEMORY_PROPERTY_DYNAMIC_STORAGE_BIT*/;
		}

		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostCoherent ) )
		{
			result = GL_MEMORY_PROPERTY_COHERENT_BIT
				| GL_MEMORY_PROPERTY_PERSISTENT_BIT;
		}

		return result;
	}
}
