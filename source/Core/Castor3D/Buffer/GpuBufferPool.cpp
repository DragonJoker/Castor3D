#include "Castor3D/Buffer/GpuBufferPool.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr minBlockSize = 96u;

		inline void copyBuffer( ashes::CommandBuffer const & commandBuffer
			, ashes::BufferBase const & src
			, ashes::BufferBase const & dst
			, VkDeviceSize offset
			, VkDeviceSize size
			, VkPipelineStageFlags flags )
		{
			auto dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, dst.makeTransferDestination() );
			commandBuffer.copyBuffer( src
				, dst
				, uint32_t( size )
				, uint32_t( offset ) );
			dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, flags
				, dst.makeUniformBufferInput() );
		}
	}

	GpuBufferPool::GpuBufferPool( RenderSystem & renderSystem
		, RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	GpuBufferPool::~GpuBufferPool()
	{
	}

	void GpuBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	GpuBuffer & GpuBufferPool::doGetBuffer( VkDeviceSize size
		, VkBufferUsageFlagBits target
		, VkMemoryPropertyFlags memory
		, MemChunk & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );

		if ( it == m_buffers.end() )
		{
			it = m_buffers.emplace( key, BufferArray{} ).first;
		}

		auto itB = doFindBuffer( size, it->second );

		if ( itB == it->second.end() )
		{
			VkDeviceSize level = 20u;
			VkDeviceSize maxSize = ( 1u << level ) * minBlockSize;

			while ( size > maxSize && level <= 24 )
			{
				++level;
				maxSize = ( 1u << level ) * minBlockSize;
			}

			CU_Require( maxSize < std::numeric_limits< uint32_t >::max() );
			CU_Require( maxSize >= size );

			std::unique_ptr< GpuBuffer > buffer = std::make_unique< GpuBuffer >( *getRenderSystem() 
				, target
				, memory
				, m_debugName
				, ashes::QueueShare{}
				, uint32_t( level )
				, minBlockSize );
			buffer->initialise( m_device );
			it->second.emplace_back( std::move( buffer ) );
			itB = std::next( it->second.begin(), it->second.size() - 1u );
		}

		chunk = ( *itB )->allocate( size );
		return *( *itB ).get();
	}

	void GpuBufferPool::doPutBuffer( GpuBuffer const & buffer
		, VkBufferUsageFlagBits target
		, VkMemoryPropertyFlags memory
		, MemChunk const & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&buffer]( std::unique_ptr< GpuBuffer > const & lookup )
			{
				return &lookup->getBuffer().getBuffer() == &buffer.getBuffer().getBuffer();
			} );
		CU_Require( itB != it->second.end() );
		( *itB )->deallocate( chunk );
	}

	GpuBufferPool::BufferArray::iterator GpuBufferPool::doFindBuffer( VkDeviceSize size
		, GpuBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable( size ) )
		{
			++it;
		}

		return it;
	}

	uint32_t GpuBufferPool::doMakeKey( VkBufferUsageFlagBits target
		, VkMemoryPropertyFlags flags )
	{
		return ( uint32_t( target ) << 0u )
			| ( uint32_t( flags ) << 16u );
	}
}
