#include "Descriptor/VkDescriptorPool.hpp"

#include "Descriptor/VkDescriptorSet.hpp"
#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Descriptor/VkDescriptorSetLayoutBinding.hpp"
#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	DescriptorPool::DescriptorPool( Device const & device
		, renderer::DescriptorPoolCreateFlags flags
		, uint32_t maxSets
		, renderer::DescriptorPoolSizeArray poolSizes )
		: renderer::DescriptorPool{ flags }
		, m_device{ device }
	{
		std::vector< VkDescriptorPoolSize > vkpoolSizes;

		for ( auto const & poolSize : poolSizes )
		{
			vkpoolSizes.push_back(
			{
				convert( poolSize.type ),
				poolSize.descriptorCount
			} );
		}

		VkDescriptorPoolCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,      // sType
			nullptr,                                            // pNext
			convert( flags ),                                   // flags
			maxSets,                                            // maxSets
			uint32_t( vkpoolSizes.size() ),                     // poolSizeCount
			vkpoolSizes.data()                                  // pPoolSizes
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

	DescriptorPool::~DescriptorPool()
	{
		m_device.vkDestroyDescriptorPool( m_device, m_pool, nullptr );
	}

	renderer::DescriptorSetPtr DescriptorPool::createDescriptorSet( renderer::DescriptorSetLayout const & layout
		, uint32_t bindingPoint )const
	{
		return std::make_unique< DescriptorSet >( m_device
			, *this
			, static_cast< DescriptorSetLayout const & >( layout )
			, bindingPoint );
	}
}
