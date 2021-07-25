#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	template< typename DataT >
	inline UniformBufferT< DataT >::UniformBufferT( RenderSystem const & renderSystem
		, VkDeviceSize count
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags flags
		, castor::String debugName
		, ashes::QueueShare sharingMode )
		: UniformBufferBase
		{
			renderSystem,
			count,
			sizeof( DataT ),
			usage,
			flags,
			std::move( debugName ),
			std::move( sharingMode ),
		}
		, m_data( count, DataT{} )
	{
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags )const
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( DataT )
			, offset
			, flags );
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, VkPipelineStageFlags flags )const
	{
		UniformBufferBase::upload( stagingBuffer
			, commandBuffer
			, getDatas().data()
			, getDatas().size() * sizeof( DataT )
			, offset
			, flags );
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, FramePassTimer const & timer
		, uint32_t index )const
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( DataT )
			, offset
			, flags
			, timer
			, index );
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::upload( ashes::BufferBase const & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, VkPipelineStageFlags flags
		, FramePassTimer const & timer
		, uint32_t index )const
	{
		UniformBufferBase::upload( stagingBuffer
			, commandBuffer
			, getDatas().data()
			, getDatas().size() * sizeof( DataT )
			, offset
			, flags
			, timer
			, index );
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::upload( VkDeviceSize offset
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
				std::memcpy( buffer, &m_data[offset + i], sizeof( DataT ) );
				buffer += size;
			}

			getBuffer().getBuffer().flush( offset * size, range * size );
			getBuffer().getBuffer().unlock();
		}
	}

	template< typename DataT >
	inline void UniformBufferT< DataT >::download( ashes::BufferBase const & stagingBuffer
		, ashes::Queue const & queue
		, ashes::CommandPool const & commandPool
		, uint32_t offset
		, VkPipelineStageFlags flags
		, FramePassTimer const & timer
		, uint32_t index )
	{
		UniformBufferBase::upload( stagingBuffer
			, queue
			, commandPool
			, getDatas().data()
			, getDatas().size() * sizeof( DataT )
			, offset
			, flags
			, timer
			, index );
	}
}
