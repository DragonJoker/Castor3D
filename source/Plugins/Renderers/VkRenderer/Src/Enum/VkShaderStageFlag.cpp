#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkShaderStageFlags convert( renderer::ShaderStageFlags const & flags )
	{
		VkShaderStageFlags result{ 0 };

		if ( checkFlag( flags, renderer::ShaderStageFlag::eVertex ) )
		{
			result |= VK_SHADER_STAGE_VERTEX_BIT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eTessellationControl ) )
		{
			result |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eTessellationEvaluation ) )
		{
			result |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eGeometry ) )
		{
			result |= VK_SHADER_STAGE_GEOMETRY_BIT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eFragment ) )
		{
			result |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eCompute ) )
		{
			result |= VK_SHADER_STAGE_COMPUTE_BIT;
		}

		return result;
	}

	VkShaderStageFlagBits convert( renderer::ShaderStageFlag const & flag )
	{
		VkShaderStageFlags result{ 0 };

		switch ( flag )
		{
		case renderer::ShaderStageFlag::eVertex:
			return VK_SHADER_STAGE_VERTEX_BIT;

		case renderer::ShaderStageFlag::eTessellationControl:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

		case renderer::ShaderStageFlag::eTessellationEvaluation:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

		case renderer::ShaderStageFlag::eGeometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;

		case renderer::ShaderStageFlag::eFragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		case renderer::ShaderStageFlag::eCompute:
			return VK_SHADER_STAGE_COMPUTE_BIT;

		default:
			assert( false );
			return VK_SHADER_STAGE_ALL;
		}
	}
}
