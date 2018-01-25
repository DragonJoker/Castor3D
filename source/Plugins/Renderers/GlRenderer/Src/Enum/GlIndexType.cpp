#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
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
