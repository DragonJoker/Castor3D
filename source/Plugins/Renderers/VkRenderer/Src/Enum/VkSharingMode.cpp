#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSharingMode convert( renderer::SharingMode const & value )
	{
		switch ( value )
		{
		case renderer::SharingMode::eExclusive:
			return VK_SHARING_MODE_EXCLUSIVE;

		case renderer::SharingMode::eConcurrent:
			return VK_SHARING_MODE_CONCURRENT;

		default:
			assert( false && "Unsupported SharingMode" );
			return VK_SHARING_MODE_EXCLUSIVE;
		}
	}
}
