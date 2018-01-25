#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSamplerMipmapMode convert( renderer::MipmapMode const & mode )
	{
		switch ( mode )
		{
		case renderer::MipmapMode::eNone:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;

		case renderer::MipmapMode::eNearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;

		case renderer::MipmapMode::eLinear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;

		default:
			assert( false );
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}
	}
}
