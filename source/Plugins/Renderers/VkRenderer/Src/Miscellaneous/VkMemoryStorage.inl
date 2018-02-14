/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#include "Core/VkDevice.hpp"
#include "Core/VkPhysicalDevice.hpp"

namespace vk_renderer
{
	//*********************************************************************************************

	namespace details
	{
		template< bool Image >
		struct MemoryRequirementsGetter;

		template<>
		struct MemoryRequirementsGetter< true >
		{
			static VkMemoryRequirements retrieve( Device const & device
				, VkImage const & image )
			{
				return device.getImageMemoryRequirements( image );
			}
		};

		template<>
		struct MemoryRequirementsGetter< false >
		{
			static VkMemoryRequirements retrieve( Device const & device
				, VkBuffer const & buffer )
			{
				return device.getBufferMemoryRequirements( buffer );
			}
		};
	}

	//*********************************************************************************************

	template< typename VkType, bool Image >
	inline MemoryStorage< VkType, Image >::MemoryStorage( Device const & device
		, VkType buffer
		, VkMemoryPropertyFlags flags )
		: m_device{ device }
	{
		using Getter = typename details::MemoryRequirementsGetter< Image >;
		VkMemoryRequirements memoryRequirements{ Getter::retrieve( device, buffer ) };
		uint32_t deducedTypeIndex{ 0xFFFFFFFF };

		if ( !m_device.getPhysicalDevice().deduceMemoryType( memoryRequirements.memoryTypeBits
			, flags
			, deducedTypeIndex ) )
		{
			throw std::runtime_error{ "Could not find an appropriate memory type for buffer storage" };
		}

		VkMemoryAllocateInfo allocateInfo
		{
			VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			nullptr,
			memoryRequirements.size,                  // allocationSize
			deducedTypeIndex                          // memoryTypeIndex
		};
		DEBUG_DUMP( allocateInfo );
		auto res = m_device.vkAllocateMemory( m_device, &allocateInfo, nullptr, &m_memory );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Memory storage allocation failed: " + getLastError() };
		}
	}

	template< typename VkType, bool Image >
	inline MemoryStorage< VkType, Image >::~MemoryStorage()
	{
		m_device.vkFreeMemory( m_device, m_memory, nullptr );
	}

	template< typename VkType, bool Image >
	inline uint8_t * MemoryStorage< VkType, Image >::lock( uint32_t offset
		, uint32_t size
		, VkMemoryMapFlags flags )const
	{
		uint8_t * pointer{ nullptr };
		auto res = m_device.vkMapMemory( m_device
			, m_memory
			, offset
			, size
			, 0u/*flags*/
			, reinterpret_cast< void ** >( &pointer ) );

		if ( !checkError( res ) )
		{
			std::cerr << "Storage memory mapping failed: " << getLastError() << std::endl;
		}

		return pointer;
	}

	template< typename VkType, bool Image >
	inline void MemoryStorage< VkType, Image >::unlock( uint32_t size
		, bool modified )const
	{
		if ( modified )
		{
			VkMappedMemoryRange flushRange
			{
				VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				nullptr,
				m_memory,                                         // memory
				0,                                                // offset
				size                                              // size
			};
			DEBUG_DUMP( flushRange );
			auto res = m_device.vkFlushMappedMemoryRanges( m_device, 1, &flushRange );

			if ( !checkError( res ) )
			{
				std::cerr << "Storage memory range flush failed: " << getLastError() << std::endl;
			}
		}

		m_device.vkUnmapMemory( m_device, m_memory );
	}

	//*********************************************************************************************
}
