/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineRasterizationStateCreateInfo convert( renderer::RasterisationState const & state )
	{
		return VkPipelineRasterizationStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			convert( state.getFlags() ),
			state.isDepthClampEnabled(),
			state.isRasteriserDiscardEnabled(),
			convert( state.getPolygonMode() ),
			convert( state.getCullMode() ),
			convert( state.getFrontFace() ),
			state.isDepthBiasEnabled(),
			state.getDepthBiasConstantFactor(),
			state.getDepthBiasClamp(),
			state.getDepthBiasSlopeFactor(),
			state.getLineWidth()
		};
	}
}
