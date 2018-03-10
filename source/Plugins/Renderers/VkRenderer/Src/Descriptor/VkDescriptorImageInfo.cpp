#include "VkRendererPrerequisites.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Image/VkSampler.hpp"
#include "Image/VkTextureView.hpp"

namespace vk_renderer
{
	VkDescriptorImageInfo convert( renderer::DescriptorImageInfo const & value )
	{
		VkDescriptorImageInfo result{};
		result.imageLayout = convert( value.imageLayout );

		if ( bool( value.imageView ) )
		{
			result.imageView = static_cast< TextureView const & >( value.imageView.value().get() );
		}

		if ( bool( value.sampler ) )
		{
			result.sampler = static_cast< Sampler const & >( value.sampler.value().get() );
		}

		return result;
	}
}
