#include "Descriptor/VkDescriptorSetPool.hpp"

#include "Descriptor/VkDescriptorSet.hpp"
#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Descriptor/VkDescriptorSetLayoutBinding.hpp"
#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	DescriptorSetPool::DescriptorSetPool( Device const & device
		, DescriptorSetLayout const & layout
		, uint32_t maxSets
		, bool automaticFree )
		: renderer::DescriptorSetPool{ layout, maxSets }
		, m_device{ device }
		, m_layout{ layout }
		, m_automaticFree{ automaticFree }
	{
		std::vector< VkDescriptorPoolSize > poolSizes;

		for ( auto const & binding : layout.getBindings() )
		{
			poolSizes.push_back(
			{
				convert( binding.getDescriptorType() ),         // type
				maxSets * binding.getDescriptorsCount()         // descriptorCount
			} );
		}

		VkDescriptorPoolCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,      // sType
			nullptr,                                            // pNext
			m_automaticFree                                     // flags
				? 0u
				: VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			maxSets,                                            // maxSets
			uint32_t( poolSizes.size() ),                       // poolSizeCount
			poolSizes.data()                                    // pPoolSizes
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateDescriptorPool( m_device
			, &createInfo
			, nullptr
			, &m_pool );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Descriptor pool creation failed: " + getLastError() };
		}
	}

	DescriptorSetPool::~DescriptorSetPool()
	{
		m_device.vkDestroyDescriptorPool( m_device, m_pool, nullptr );
	}

	renderer::DescriptorSetPtr DescriptorSetPool::createDescriptorSet( uint32_t bindingPoint )const
	{
		return std::make_unique< DescriptorSet >( m_device
			, *this
			, bindingPoint );
	}
}
