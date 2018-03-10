/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineInputAssemblyStateCreateInfo convert( renderer::InputAssemblyState const & state )
	{
		return VkPipelineInputAssemblyStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			convert( state.topology ),                               // topology
			state.primitiveRestartEnable                             // primitiveRestartEnable
		};
	}
}
