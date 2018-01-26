#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlBufferTarget value )
	{
		switch ( value )
		{
		case gl_renderer::GL_BUFFER_TARGET_ARRAY:
			return "GL_ARRAY_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_ELEMENT_ARRAY:
			return "GL_ELEMENT_ARRAY_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_PIXEL_PACK:
			return "GL_PIXEL_PACK_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_PIXEL_UNPACK:
			return "GL_PIXEL_UNPACK_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_UNIFORM:
			return "GL_UNIFORM_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_TEXTURE:
			return "GL_TEXTURE_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_COPY_READ:
			return "GL_COPY_READ_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_COPY_WRITE:
			return "GL_COPY_WRITE_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_DRAW_INDIRECT:
			return "GL_DRAW_INDIRECT_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_SHADER_STORAGE:
			return "GL_SHADER_STORAGE_BUFFER";

		case gl_renderer::GL_BUFFER_TARGET_DISPATCH_INDIRECT:
			return "GL_DISPATCH_INDIRECT_BUFFER";

		default:
			assert( false && "Unsupported GlBufferTarget" );
			return "GlBufferTarget_UNKNOWN";
		}
	}

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
