#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	//*********************************************************************************************

	template< typename T >
	inline UniformBuffer< T >::UniformBuffer( RenderSystem const & renderSystem
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: UniformBufferBase
		{
			renderSystem,
			count,
			sizeof( T ),
			usage,
			flags,
			std::move( debugName ),
			std::move( sharingMode ),
		}
		, m_data( count, T{} )
	{
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags )const
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( T )
			, offset
			, flags );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, VkPipelineStageFlags flags )const
	{
		UniformBufferBase::upload( stagingBuffer
			, commandBuffer
			, getDatas().data()
			, getDatas().size() * sizeof( T )
			, offset
			, flags );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( T )
			, offset
			, flags
			, timer
			, index );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		UniformBufferBase::upload( stagingBuffer
			, commandBuffer
			, getDatas().data()
			, getDatas().size() * sizeof( T )
			, offset
			, flags
			, timer
			, index );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( VkDeviceSize offset
		, VkDeviceSize range )const
	{
		assert( range + offset <= m_data.size() );
		auto size = getBuffer().getAlignedSize();

		if ( auto buffer = getBuffer().getBuffer().lock( offset * size
			, range * size
			, 0u ) )
		{
			for ( auto i = 0u; i < range; ++i )
			{
				std::memcpy( buffer, &m_data[offset + i], sizeof( T ) );
				buffer += size;
			}

			getBuffer().getBuffer().flush( offset * size, range * size );
			getBuffer().getBuffer().unlock();
		}
	}

	template< typename T >
	inline void UniformBuffer< T >::download( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( T )
			, offset
			, flags
			, timer
			, index );
	}

	//*********************************************************************************************

	template< typename T >
	inline PoolUniformBuffer< T >::PoolUniformBuffer( RenderSystem const & renderSystem
		, castor::ArrayView< T > data
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: UniformBufferBase
		{
			renderSystem,
			data.size(),
			sizeof( T ),
			usage,
			flags,
			std::move( debugName ),
			std::move( sharingMode ),
		}
		, m_data{ std::move( data ) }
	{
	}

	//*********************************************************************************************
}
