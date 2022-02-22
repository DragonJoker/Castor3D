#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementCUSmartPtr( castor3d, ObjectBufferPool )
CU_ImplementCUSmartPtr( castor3d, SkinnedObjectBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		template< typename DataT >
		GpuPackedBuffer createBuffer( RenderDevice const & device
			, VkDeviceSize count
			, VkBufferUsageFlags usage
			, std::string debugName )
		{
			VkDeviceSize maxCount = 1'000'000;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			GpuPackedBuffer result{ device.renderSystem
				, usage
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ) } };
			result.initialise( device );
			return result;
		}
	}

	//*********************************************************************************************

	ObjectBufferPool::ObjectBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	void ObjectBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto it = doFindBuffer( vertexCount, indexCount, m_buffers );

		if ( it == m_buffers.end() )
		{
			auto buffers = std::make_unique< ModelBuffers >( createBuffer< InterleavedVertex >( m_device
					, vertexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Vertex" )
				, createBuffer< uint32_t >( m_device
					, indexCount
					, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Index" ) );
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.vtxBuffer = &( *it )->vertex.getBuffer().getBuffer();
		result.vtxChunk = ( *it )->vertex.allocate( sizeof( InterleavedVertex ) * vertexCount );

		if ( indexCount )
		{
			result.idxBuffer = &( *it )->index.getBuffer().getBuffer();
			result.idxChunk = ( *it )->index.allocate( sizeof( uint32_t ) * indexCount );
		}

		return result;
	}

	void ObjectBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&bufferOffset]( std::unique_ptr< ModelBuffers > const & lookup )
			{
				bool result = &lookup->vertex.getBuffer().getBuffer() == &bufferOffset.getVertexBuffer();

				if ( result && bufferOffset.idxBuffer )
				{
					result = &lookup->index.getBuffer().getBuffer() == &bufferOffset.getIndexBuffer();
				}

				return result;
			} );
		CU_Require( it != m_buffers.end() );
		( *it )->vertex.deallocate( bufferOffset.vtxChunk );

		if ( bufferOffset.idxBuffer )
		{
			( *it )->index.deallocate( bufferOffset.idxChunk );
		}
	}

	ObjectBufferPool::BufferArray::iterator ObjectBufferPool::doFindBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, ObjectBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end()
			&& ( !( *it )->vertex.hasAvailable( sizeof( InterleavedVertex ) * vertexCount )
				|| !( *it )->index.hasAvailable( sizeof( uint32_t ) * indexCount ) ) )
		{
			++it;
		}

		return it;
	}

	//*********************************************************************************************

	SkinnedObjectBufferPool::SkinnedObjectBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	void SkinnedObjectBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	ObjectBufferOffset SkinnedObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto it = doFindBuffer( vertexCount, indexCount, m_buffers );

		if ( it == m_buffers.end() )
		{
			auto buffers = std::make_unique< ModelBuffers >( createBuffer< InterleavedVertex >( m_device
					, vertexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Vertex" )
				, createBuffer< VertexBoneData >( m_device
					, indexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Bones" )
				, createBuffer< uint32_t >( m_device
					, indexCount
					, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Index" ) );
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.vtxBuffer = &( *it )->vertex.getBuffer().getBuffer();
		result.vtxChunk = ( *it )->vertex.allocate( sizeof( InterleavedVertex ) * vertexCount );
		result.bonBuffer = &( *it )->bones.getBuffer().getBuffer();
		result.bonChunk = ( *it )->bones.allocate( sizeof( VertexBoneData ) * vertexCount );

		if ( indexCount )
		{
			result.idxBuffer = &( *it )->index.getBuffer().getBuffer();
			result.idxChunk = ( *it )->index.allocate( sizeof( uint32_t ) * indexCount );
		}

		return result;
	}

	void SkinnedObjectBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&bufferOffset]( std::unique_ptr< ModelBuffers > const & lookup )
			{
				bool result = &lookup->vertex.getBuffer().getBuffer() == &bufferOffset.getVertexBuffer();

				if ( result && bufferOffset.idxBuffer )
				{
					result = &lookup->index.getBuffer().getBuffer() == &bufferOffset.getIndexBuffer();
				}

				if ( result && bufferOffset.bonBuffer )
				{
					result = &lookup->bones.getBuffer().getBuffer() == &bufferOffset.getBonesBuffer();
				}

				return result;
			} );
		CU_Require( it != m_buffers.end() );
		( *it )->vertex.deallocate( bufferOffset.vtxChunk );
		( *it )->bones.deallocate( bufferOffset.bonChunk );

		if ( bufferOffset.idxBuffer )
		{
			( *it )->index.deallocate( bufferOffset.idxChunk );
		}
	}

	SkinnedObjectBufferPool::BufferArray::iterator SkinnedObjectBufferPool::doFindBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, SkinnedObjectBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end()
			&& ( !( *it )->vertex.hasAvailable( sizeof( InterleavedVertex ) * vertexCount )
				|| !( *it )->index.hasAvailable( sizeof( uint32_t ) * indexCount ) ) )
		{
			++it;
		}

		return it;
	}

	//*********************************************************************************************
}
