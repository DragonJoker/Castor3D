#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlBufferTarget convert( renderer::BufferTargets const & targets )
	{
		GlBufferTarget result{ GlBufferTarget( 0 ) };

		if ( checkFlag( targets, renderer::BufferTarget::eUniformTexelBuffer ) )
		{
			result = GL_BUFFER_TARGET_TEXTURE;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eStorageTexelBuffer ) )
		{
			result = GL_BUFFER_TARGET_TEXTURE;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eUniformBuffer ) )
		{
			result = GL_BUFFER_TARGET_UNIFORM;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eStorageBuffer ) )
		{
			result = GL_BUFFER_TARGET_SHADER_STORAGE;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eIndexBuffer ) )
		{
			result = GL_BUFFER_TARGET_ELEMENT_ARRAY;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eVertexBuffer ) )
		{
			result = GL_BUFFER_TARGET_ARRAY;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eDispatchIndirectBuffer ) )
		{
			result = GL_BUFFER_TARGET_DISPATCH_INDIRECT;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eDrawIndirectBuffer ) )
		{
			result = GL_BUFFER_TARGET_DRAW_INDIRECT;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::ePackBuffer ) )
		{
			result = GL_BUFFER_TARGET_PIXEL_PACK;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eUnpackBuffer ) )
		{
			result = GL_BUFFER_TARGET_PIXEL_UNPACK;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eTransferSrc ) )
		{
			result = GL_BUFFER_TARGET_COPY_READ;
		}
		else if ( checkFlag( targets, renderer::BufferTarget::eTransferDst ) )
		{
			result = GL_BUFFER_TARGET_COPY_WRITE;
		}

		return result;
	}
}
