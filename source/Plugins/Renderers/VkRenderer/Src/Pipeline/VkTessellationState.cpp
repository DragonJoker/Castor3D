/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineTessellationStateCreateInfo convert( renderer::TessellationState const & state )
	{
		return VkPipelineTessellationStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			nullptr,
			convert( state.getFlags() ),
			state.getControlPoints()
		};
	}
}
