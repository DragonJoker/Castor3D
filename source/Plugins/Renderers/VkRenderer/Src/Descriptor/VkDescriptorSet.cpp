#include "Descriptor/VkDescriptorSet.hpp"

#include "Buffer/VkBuffer.hpp"
#include "Buffer/VkBufferView.hpp"
#include "Buffer/VkUniformBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Descriptor/VkDescriptorSetLayoutBinding.hpp"
#include "Descriptor/VkDescriptorPool.hpp"
#include "Image/VkSampler.hpp"
#include "Image/VkTextureView.hpp"

namespace vk_renderer
{
	DescriptorSet::DescriptorSet( Device const & device
		, DescriptorPool const & pool
		, DescriptorSetLayout const & layout
		, uint32_t bindingPoint )
		: renderer::DescriptorSet{ pool, bindingPoint }
		, m_device{ device }
		, m_pool{ pool }
		, m_layout{ layout }
	{
		VkDescriptorSetAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,           // sType
			nullptr,                                                  // pNext
			pool,                                                     // descriptorPool
			1u,                                                       // descriptorSetCount
			&static_cast< VkDescriptorSetLayout const & >( m_layout ) // pSetLayouts
		};
		DEBUG_DUMP( allocateInfo );
		auto res = m_device.vkAllocateDescriptorSets( m_device
			, &allocateInfo
			, &m_descriptorSet );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Descriptor set allocation failed: " + getLastError() };
		}
	}

	DescriptorSet::~DescriptorSet()
	{
		if ( !m_pool.hasAutomaticFree() )
		{
			m_device.vkFreeDescriptorSets( m_device
				, m_pool
				, 1u
				, &m_descriptorSet );
		}
	}

	void DescriptorSet::update()const
	{
		std::list< VkDescriptorImageInfo > imageInfos;
		std::list< VkDescriptorBufferInfo > bufferInfos;
		std::list< VkBufferView > texelBufferViews;
		std::vector< VkWriteDescriptorSet > writes;

		for ( auto & write : m_writes )
		{
			writes.push_back( convert( write
			, *this
			, imageInfos
			, bufferInfos
			, texelBufferViews ) );
		}

		DEBUG_DUMP( writes );
		m_device.vkUpdateDescriptorSets( m_device
			, static_cast< uint32_t >( writes.size() )
			, writes.data()
			, 0
			, nullptr );
	}
}
