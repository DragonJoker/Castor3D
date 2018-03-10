#include "VkRendererPrerequisites.hpp"

#include "Buffer/VkBufferView.hpp"
#include "Descriptor/VkDescriptorSet.hpp"

namespace vk_renderer
{
	VkWriteDescriptorSet convert( renderer::WriteDescriptorSet const & value
		, DescriptorSet const & descriptorSet
		, std::list< VkDescriptorImageInfo > & imageInfos
		, std::list< VkDescriptorBufferInfo > & bufferInfos
		, std::list< VkBufferView > & texelBufferViews )
	{
		VkWriteDescriptorSet result
		{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			descriptorSet,
			value.dstBinding,
			value.dstArrayElement,
			value.descriptorCount,
			convert( value.descriptorType ),
			nullptr,
			nullptr,
			nullptr,
		};

		if ( bool( value.imageInfo ) )
		{
			imageInfos.push_back( convert( value.imageInfo.value() ) );
			result.pImageInfo = &imageInfos.back();
		}

		if ( bool( value.bufferInfo ) )
		{
			bufferInfos.push_back( convert( value.bufferInfo.value() ) );
			result.pBufferInfo = &bufferInfos.back();
		}

		if ( bool( value.texelBufferView ) )
		{
			auto & texelBufferView = static_cast< BufferView const & >( value.texelBufferView.value().get() );
			result.pTexelBufferView = &static_cast< VkBufferView const & >( texelBufferView );
		}

		return result;
	}
}
