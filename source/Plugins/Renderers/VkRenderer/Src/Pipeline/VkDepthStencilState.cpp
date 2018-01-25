/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineDepthStencilStateCreateInfo convert( renderer::DepthStencilState const & state )
	{
		return VkPipelineDepthStencilStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			convert( state.getFlags() ),
			state.isDepthTestEnabled(),
			state.isDepthWriteEnabled(),
			convert( state.getDepthCompareOp() ),
			state.isDepthBoundsTestEnabled(),
			state.isStencilTestEnabled(),
			convert( state.getFrontStencilOp() ),
			convert( state.getBackStencilOp() ),
			state.getMinDepthBounds(),
			state.getMaxDepthBounds()
		};
	}
}
