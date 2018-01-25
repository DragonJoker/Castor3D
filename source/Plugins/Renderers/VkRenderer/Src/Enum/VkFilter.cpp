#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkFilter convert( renderer::Filter const & filter )
	{
		switch ( filter )
		{
		case renderer::Filter::eNearest:
			return VK_FILTER_NEAREST;

		case renderer::Filter::eLinear:
			return VK_FILTER_LINEAR;

		default:
			assert( false );
			return VK_FILTER_NEAREST;
		}
	}

}
