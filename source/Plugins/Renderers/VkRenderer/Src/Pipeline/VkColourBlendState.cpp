/*
This file belongs to RendererLib.
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
			state.logicOpEnable,                                     // logicOpEnable
			convert( state.logicOp ),                                // logicOp
			uint32_t( attaches.size() ),                                  // attachmentCount
			attaches.data(),                                              // pAttachments
			{                                                             // blendConstants[4]
				state.blendConstants[0],
				state.blendConstants[1],
				state.blendConstants[2],
				state.blendConstants[3],
			}
		};
	}
}
