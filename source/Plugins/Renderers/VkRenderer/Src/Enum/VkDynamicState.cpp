#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkDynamicState convert( renderer::DynamicState const & value )
	{
		switch ( value )
		{
		case renderer::DynamicState::eViewport:
			return VK_DYNAMIC_STATE_VIEWPORT;
		case renderer::DynamicState::eScissor:
			return VK_DYNAMIC_STATE_SCISSOR;
		case renderer::DynamicState::eLineWidth:
			return VK_DYNAMIC_STATE_LINE_WIDTH;
		case renderer::DynamicState::eDepthBias:
			return VK_DYNAMIC_STATE_DEPTH_BIAS;
		case renderer::DynamicState::eBlendConstants:
			return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
		case renderer::DynamicState::eDepthBounds:
			return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
		case renderer::DynamicState::eStencilCompareMask:
			return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
		case renderer::DynamicState::eStencilWriteMask:
			return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
		case renderer::DynamicState::eStencilReference:
			return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
		default:
			assert( false && "Unsupported DynamicState" );
			return VK_DYNAMIC_STATE_VIEWPORT;
			break;
		}
	}
}
