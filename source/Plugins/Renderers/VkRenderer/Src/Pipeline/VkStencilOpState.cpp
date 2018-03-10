/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkStencilOpState convert( renderer::StencilOpState const & state )
	{
		return VkStencilOpState
		{
			convert( state.failOp ),
			convert( state.passOp ),
			convert( state.depthFailOp ),
			convert( state.compareOp ),
			state.compareMask,
			state.writeMask,
			state.reference,
		};
	}
}
