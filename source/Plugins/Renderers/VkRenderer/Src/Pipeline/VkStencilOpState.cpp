/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkStencilOpState convert( renderer::StencilOpState const & state )
	{
		return VkStencilOpState
		{
			convert( state.getFailOp() ),
			convert( state.getPassOp() ),
			convert( state.getDepthFailOp() ),
			convert( state.getCompareOp() ),
			state.getCompareMask(),
			state.getWriteMask(),
			state.getReference(),
		};
	}
}
