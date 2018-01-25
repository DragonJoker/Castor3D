/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineColorBlendAttachmentState convert( renderer::ColourBlendStateAttachment const & state )
	{
		return VkPipelineColorBlendAttachmentState
		{
			state.isBlendEnabled(),
			convert( state.getSrcColourBlendFactor() ),
			convert( state.getDstColourBlendFactor() ),
			convert( state.getColourBlendOp() ),
			convert( state.getSrcAlphaBlendFactor() ),
			convert( state.getDstAlphaBlendFactor() ),
			convert( state.getAlphaBlendOp() ),
			convert( state.getColourWriteMask() ),
		};
	}
}
