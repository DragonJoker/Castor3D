/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/VkRenderSubpassState.hpp"

namespace vk_renderer
{
	RenderSubpassState convert( renderer::RenderSubpassState const & state )
	{
		return RenderSubpassState
		{
			convert( state.getPipelineStage() ),
			convert( state.getAccess() )
		};
	}
}
