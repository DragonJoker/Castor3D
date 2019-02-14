#include "Render/RenderSystem.hpp"

#include "Render/RenderPassTimer.hpp"

#include <Ashes/Buffer/StagingBuffer.hpp>
#include <Ashes/Command/CommandBuffer.hpp>
#include <Ashes/Core/Device.hpp>
#include <Ashes/Sync/BufferMemoryBarrier.hpp>

#include <algorithm>

namespace castor3d
{
	template< typename T >
	inline UniformBuffer< T >::UniformBuffer( RenderSystem const & renderSystem
		, uint32_t count
		, ashes::MemoryPropertyFlags flags )
		: m_renderSystem{ renderSystem }
		, m_flags{ flags }
		, m_count{ count }
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			m_available.insert( i );
		}
	}

	template< typename T >
	inline UniformBuffer< T >::~UniformBuffer()
	{
	}

	template< typename T >
	inline void UniformBuffer< T >::initialise()
	{
		CU_Require( m_renderSystem.hasCurrentDevice() );
		m_buffer = ashes::makeUniformBuffer< T >( getCurrentDevice( m_renderSystem )
			, m_count
			, ashes::BufferTarget::eTransferDst
			, m_flags );
	}

	template< typename T >
	inline void UniformBuffer< T >::cleanup()
	{
		CU_Require( m_renderSystem.hasCurrentDevice() );
		m_buffer.reset();
	}

	template< typename T >
	inline bool UniformBuffer< T >::hasAvailable()const
	{
		return !m_available.empty();
	}

	template< typename T >
	inline uint32_t UniformBuffer< T >::allocate()
	{
		CU_Require( hasAvailable() );
		uint32_t result = *m_available.begin();
		m_available.erase( m_available.begin() );
		return result;
	}

	template< typename T >
	inline void UniformBuffer< T >::deallocate( uint32_t offset )
	{
		m_available.insert( offset );
	}

	template< typename T >
	inline void UniformBuffer< T >::upload( ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, ashes::PipelineStageFlags flags
		, RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto elemAlignedSize = getBuffer().getAlignedSize();
		auto data = getBuffer().getDatas().data();

		if ( auto dest = stagingBuffer.getBuffer().lock( 0u
			, m_count * elemAlignedSize
			, ashes::MemoryMapFlag::eWrite | ashes::MemoryMapFlag::eInvalidateBuffer ) )
		{
			auto buffer = dest;

			for ( auto & data : getBuffer().getDatas() )
			{
				std::memcpy( buffer, &data, sizeof( T ) );
				buffer += elemAlignedSize;
			}

			stagingBuffer.getBuffer().flush( 0u, m_count * elemAlignedSize );
			stagingBuffer.getBuffer().unlock();
		}

		commandBuffer.begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		timer.beginPass( commandBuffer, index );
		commandBuffer.memoryBarrier( ashes::PipelineStageFlag::eTransfer
			, ashes::PipelineStageFlag::eTransfer
			, stagingBuffer.getBuffer().makeTransferSource() );
		auto srcStageFlags = getBuffer().getUbo().getBuffer().getCompatibleStageFlags();
		commandBuffer.memoryBarrier( srcStageFlags
			, ashes::PipelineStageFlag::eTransfer
			, getBuffer().getUbo().getBuffer().makeTransferDestination() );
		commandBuffer.copyBuffer( stagingBuffer.getBuffer()
			, getBuffer().getUbo().getBuffer()
			, elemAlignedSize * m_count
			, elemAlignedSize * offset );
		commandBuffer.memoryBarrier( ashes::PipelineStageFlag::eTransfer
			, flags
			, getBuffer().getUbo().getBuffer().makeUniformBufferInput() );
		timer.endPass( commandBuffer, index );
		commandBuffer.end();
	}

	template< typename T >
	inline void UniformBuffer< T >::download( ashes::StagingBuffer & stagingBuffer
		, ashes::CommandBuffer const & commandBuffer
		, uint32_t offset
		, ashes::PipelineStageFlags flags )const
	{
		stagingBuffer.downloadUniformData( commandBuffer
			, getBuffer().getDatas()
			, offset
			, getBuffer()
			, flags );
	}
}
