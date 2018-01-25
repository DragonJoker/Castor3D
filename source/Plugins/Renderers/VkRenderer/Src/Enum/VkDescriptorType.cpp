#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkDescriptorType convert( renderer::DescriptorType const & type )
	{
		switch ( type )
		{
		case renderer::DescriptorType::eSampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;

		case renderer::DescriptorType::eCombinedImageSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		case renderer::DescriptorType::eSampledImage:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

		case renderer::DescriptorType::eStorageImage:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

		case renderer::DescriptorType::eUniformTexelBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

		case renderer::DescriptorType::eStorageTexelBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

		case renderer::DescriptorType::eUniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		case renderer::DescriptorType::eStorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

		case renderer::DescriptorType::eUniformBufferDynamic:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

		case renderer::DescriptorType::eStorageBufferDynamic:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

		case renderer::DescriptorType::eInputAttachment:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

		default:
			assert( false );
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		}
	}
}
