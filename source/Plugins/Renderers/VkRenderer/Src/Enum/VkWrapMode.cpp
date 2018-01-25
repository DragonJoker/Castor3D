#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSamplerAddressMode convert( renderer::WrapMode const & mode )
	{
		switch ( mode )
		{
		case renderer::WrapMode::eRepeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;

		case renderer::WrapMode::eMirroredRepeat:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;

		case renderer::WrapMode::eClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		case renderer::WrapMode::eClampToBorder:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

		case renderer::WrapMode::eMirrorClampToEdge:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;

		default:
			assert( false );
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}
}
