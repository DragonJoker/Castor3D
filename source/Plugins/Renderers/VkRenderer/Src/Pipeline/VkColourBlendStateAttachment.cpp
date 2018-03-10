/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineColorBlendAttachmentState convert( renderer::ColourBlendStateAttachment const & state )
	{
		return VkPipelineColorBlendAttachmentState
		{
			state.blendEnable,
			convert( state.srcColorBlendFactor ),
			convert( state.dstColorBlendFactor ),
			convert( state.colorBlendOp ),
			convert( state.srcAlphaBlendFactor ),
			convert( state.dstAlphaBlendFactor ),
			convert( state.alphaBlendOp ),
			convert( state.colorWriteMask ),
		};
	}
}
