#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkComponentSwizzle convert( renderer::ComponentSwizzle const & value )
	{
		switch ( value )
		{
		case renderer::ComponentSwizzle::eIdentity:
			return VK_COMPONENT_SWIZZLE_IDENTITY;

		case renderer::ComponentSwizzle::eZero:
			return VK_COMPONENT_SWIZZLE_ZERO;

		case renderer::ComponentSwizzle::eOne:
			return VK_COMPONENT_SWIZZLE_ONE;

		case renderer::ComponentSwizzle::eR:
			return VK_COMPONENT_SWIZZLE_R;

		case renderer::ComponentSwizzle::eG:
			return VK_COMPONENT_SWIZZLE_G;

		case renderer::ComponentSwizzle::eB:
			return VK_COMPONENT_SWIZZLE_B;

		case renderer::ComponentSwizzle::eA:
			return VK_COMPONENT_SWIZZLE_A;

		default:
			assert( false && "Unsupported ComponentSwizzle" );
			return VK_COMPONENT_SWIZZLE_IDENTITY;
		}
	}
}
