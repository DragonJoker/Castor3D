#include "Descriptor/VkDescriptorSetPool.hpp"

#include "Descriptor/VkDescriptorSet.hpp"
#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Descriptor/VkDescriptorSetLayoutBinding.hpp"
#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	DescriptorSetPool::DescriptorSetPool( Device const & device
		, DescriptorSetLayout const & layout
		, uint32_t maxSets )
		: renderer::DescriptorSetPool{ layout, maxSets }
		, m_device{ device }
		, m_layout{ layout }
	{
		std::vector< VkDescriptorPoolSize > poolSizes;

		for ( auto const & binding : layout.getBindings() )
		{
			poolSizes.push_back(
			{
				convert( binding.getDescriptorType() ),         // type
				maxSets                                         // descriptorCount
			} );
		}

		VkDescriptorPoolCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,      // sType
			nullptr,                                            // pNext
			0,                                                  // flags
			maxSets,                                            // maxSets
			uint32_t( poolSizes.size() ),                       // poolSizeCount
			poolSizes.data()                                    // pPoolSizes
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateDescriptorPool( m_device
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
		vk::DestroyDescriptorPool( m_device, m_pool, nullptr );
	}

	renderer::DescriptorSetPtr DescriptorSetPool::createDescriptorSet()const
	{
		return std::make_unique< DescriptorSet >( m_device
			, *this );
	}
}
