/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

#include "Shader/VkShaderModule.hpp"

namespace vk_renderer
{
	VkPipelineShaderStageCreateInfo convert( renderer::ShaderStageState const & state
		, VkSpecializationInfo const * specialisationInfo )
	{
		auto & module = static_cast< ShaderModule const & >( *state.module );
		return VkPipelineShaderStageCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			module.getStage(),
			VkShaderModule( module ),
			state.entryPoint.c_str(),
			specialisationInfo,
		};
	}
}
