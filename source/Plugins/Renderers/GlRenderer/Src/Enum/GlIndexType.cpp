#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlIndexType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_INDEX_TYPE_UINT16:
			return "GL_UNSIGNED_SHORT";

		case gl_renderer::GL_INDEX_TYPE_UINT32:
			return "GL_UNSIGNED_INT";

		default:
			assert( false && "Unsupported GlIndexType" );
			return "GlIndexType_UNKNOWN";
		}
	}

	GlIndexType convert( renderer::IndexType const & type )
	{
		switch ( type )
		{
		case renderer::IndexType::eUInt16:
			return GL_INDEX_TYPE_UINT16;

		case renderer::IndexType::eUInt32:
			return GL_INDEX_TYPE_UINT32;

		default:
			assert( false && "Unsupported index type" );
			return GL_INDEX_TYPE_UINT32;
		}
	}
}
