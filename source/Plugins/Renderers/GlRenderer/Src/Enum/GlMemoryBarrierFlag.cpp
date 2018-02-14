#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlMemoryBarrierFlags value )
	{
		std::string result;
		std::string sep;

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_VERTEX_ATTRIB_ARRAY ) )
		{
			result += sep + "GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_ELEMENT_ARRAY ) )
		{
			result += sep + "GL_ELEMENT_ARRAY_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_UNIFORM ) )
		{
			result += sep + "GL_UNIFORM_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_TEXTURE_FETCH ) )
		{
			result += sep + "GL_TEXTURE_FETCH_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_SHADER_IMAGE_ACCESS ) )
		{
			result += sep + "GL_SHADER_IMAGE_ACCESS_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_COMMAND ) )
		{
			result += sep + "GL_COMMAND_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_PIXEL_BUFFER ) )
		{
			result += sep + "GL_PIXEL_BUFFER_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_TEXTURE_UPDATE ) )
		{
			result += sep + "GL_TEXTURE_UPDATE_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_BUFFER_UPDATE ) )
		{
			result += sep + "GL_BUFFER_UPDATE_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_FRAMEBUFFER ) )
		{
			result += sep + "GL_FRAMEBUFFER_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_TRANSFORM_FEEDBACK ) )
		{
			result += sep + "GL_TRANSFORM_FEEDBACK_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_ATOMIC_COUNTER ) )
		{
			result += sep + "GL_ATOMIC_COUNTER_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_SHADER_STORAGE ) )
		{
			result += sep + "GL_SHADER_STORAGE_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_CLIENT_MAPPED_BUFFER ) )
		{
			result += sep + "GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT";
			sep = " | ";
		}

		if ( checkFlag( value, GlMemoryBarrierFlag::GL_MEMORY_BARRIER_QUERY_BUFFER ) )
		{
			result += sep + "GL_QUERY_BUFFER_BARRIER_BIT";
		}

		return result;
	}

	GlMemoryBarrierFlags convert( renderer::PipelineStageFlags const & flags )
	{
		GlMemoryBarrierFlags result{ 0 };

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTopOfPipe ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eDrawIndirect ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eVertexInput ) )
		{
			result |= GL_MEMORY_BARRIER_VERTEX_ATTRIB_ARRAY;
			result |= GL_MEMORY_BARRIER_ELEMENT_ARRAY;
			result |= GL_MEMORY_BARRIER_UNIFORM;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eVertexShader ) )
		{
			result |= GL_MEMORY_BARRIER_VERTEX_ATTRIB_ARRAY;
			result |= GL_MEMORY_BARRIER_ELEMENT_ARRAY;
			result |= GL_MEMORY_BARRIER_UNIFORM;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTessellationControlShader ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTessellationEvaluationShader ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eGeometryShader ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eFragmentShader ) )
		{
			result |= GL_MEMORY_BARRIER_TEXTURE_FETCH;
			result |= GL_MEMORY_BARRIER_UNIFORM;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eEarlyFragmentTests ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eLateFragmentTests ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eColourAttachmentOutput ) )
		{
			result |= GL_MEMORY_BARRIER_FRAMEBUFFER;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eComputeShader ) )
		{
			result |= GL_MEMORY_BARRIER_UNIFORM;
			result |= GL_MEMORY_BARRIER_SHADER_STORAGE;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTransfer ) )
		{
			result |= GL_MEMORY_BARRIER_TEXTURE_UPDATE;
			result |= GL_MEMORY_BARRIER_BUFFER_UPDATE;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eBottomOfPipe ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eHost ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eAllGraphics ) )
		{
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eAllCommands ) )
		{
			result |= GL_MEMORY_BARRIER_COMMAND;
		}

		return result;
	}
}
