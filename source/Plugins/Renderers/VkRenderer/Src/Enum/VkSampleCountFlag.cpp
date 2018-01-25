#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSampleCountFlagBits convert( renderer::SampleCountFlag const & flags )
	{
		switch ( flags )
		{
		case renderer::SampleCountFlag::e1:
			return VK_SAMPLE_COUNT_1_BIT;

		case renderer::SampleCountFlag::e2:
			return VK_SAMPLE_COUNT_2_BIT;

		case renderer::SampleCountFlag::e4:
			return VK_SAMPLE_COUNT_4_BIT;

		case renderer::SampleCountFlag::e8:
			return VK_SAMPLE_COUNT_8_BIT;

		case renderer::SampleCountFlag::e16:
			return VK_SAMPLE_COUNT_16_BIT;

		case renderer::SampleCountFlag::e32:
			return VK_SAMPLE_COUNT_32_BIT;

		case renderer::SampleCountFlag::e64:
			return VK_SAMPLE_COUNT_64_BIT;

		default:
			assert( false && "Unsupported sample count flag." );
			return VK_SAMPLE_COUNT_1_BIT;
		}
	}
}
