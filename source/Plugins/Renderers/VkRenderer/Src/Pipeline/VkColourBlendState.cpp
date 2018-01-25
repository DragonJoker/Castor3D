/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineColorBlendStateCreateInfo convert( renderer::ColourBlendState const & state
		, std::vector< VkPipelineColorBlendAttachmentState > const & attaches )
	{
		return VkPipelineColorBlendStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			state.isLogicOpEnabled(),                                     // logicOpEnable
			convert( state.getLogicOp() ),                                // logicOp
			uint32_t( attaches.size() ),                                  // attachmentCount
			attaches.data(),                                              // pAttachments
			{                                                             // blendConstants[4]
				state.getBlendConstants()[0],
				state.getBlendConstants()[1],
				state.getBlendConstants()[2],
				state.getBlendConstants()[3],
			}
		};
	}
}
