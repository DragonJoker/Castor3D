#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineStageFlags convert( renderer::PipelineStageFlags const & flags )
	{
		VkPipelineStageFlags result{ 0 };

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTopOfPipe ) )
		{
			result |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eDrawIndirect ) )
		{
			result |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eVertexInput ) )
		{
			result |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eVertexShader ) )
		{
			result |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTessellationControlShader ) )
		{
			result |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTessellationEvaluationShader ) )
		{
			result |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eGeometryShader ) )
		{
			result |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eFragmentShader ) )
		{
			result |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eEarlyFragmentTests ) )
		{
			result |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eLateFragmentTests ) )
		{
			result |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eColourAttachmentOutput ) )
		{
			result |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eComputeShader ) )
		{
			result |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eTransfer ) )
		{
			result |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eBottomOfPipe ) )
		{
			result |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eHost ) )
		{
			result |= VK_PIPELINE_STAGE_HOST_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eAllGraphics ) )
		{
			result |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		}

		if ( checkFlag( flags, renderer::PipelineStageFlag::eAllCommands ) )
		{
			result |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		return result;
	}

	VkPipelineStageFlagBits convert( renderer::PipelineStageFlag const & flag )
	{
		switch ( flag )
		{
		case renderer::PipelineStageFlag::eTopOfPipe:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		case renderer::PipelineStageFlag::eDrawIndirect:
			return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		case renderer::PipelineStageFlag::eVertexInput:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		case renderer::PipelineStageFlag::eVertexShader:
			return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

		case renderer::PipelineStageFlag::eTessellationControlShader:
			return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;

		case renderer::PipelineStageFlag::eTessellationEvaluationShader:
			return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;

		case renderer::PipelineStageFlag::eGeometryShader:
			return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;

		case renderer::PipelineStageFlag::eFragmentShader:
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		case renderer::PipelineStageFlag::eEarlyFragmentTests:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		case renderer::PipelineStageFlag::eLateFragmentTests:
			return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		case renderer::PipelineStageFlag::eColourAttachmentOutput:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		case renderer::PipelineStageFlag::eComputeShader:
			return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

		case renderer::PipelineStageFlag::eTransfer:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;

		case renderer::PipelineStageFlag::eBottomOfPipe:
			return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		case renderer::PipelineStageFlag::eHost:
			return VK_PIPELINE_STAGE_HOST_BIT;

		case renderer::PipelineStageFlag::eAllGraphics:
			return VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

		case renderer::PipelineStageFlag::eAllCommands:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		default:
			assert( false );
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
	}
}
