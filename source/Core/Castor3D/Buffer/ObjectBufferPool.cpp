#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementCUSmartPtr( castor3d, VertexBufferPool )
CU_ImplementCUSmartPtr( castor3d, ObjectBufferPool )
CU_ImplementCUSmartPtr( castor3d, SkinnedObjectBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	VertexBufferPool::VertexBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ std::move( debugName ) }
	{
	}

	void VertexBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	void VertexBufferPool::upload( ashes::CommandBuffer const & commandBuffer )
	{
		for ( auto & buffer : m_buffers )
		{
			buffer->vertex.upload( commandBuffer );
		}
	}

	void VertexBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&bufferOffset]( std::unique_ptr< ModelBuffers > const & lookup )
			{
				return &lookup->vertex.getBuffer().getBuffer() == &bufferOffset.getVertexBuffer();
			} );
		CU_Require( it != m_buffers.end() );
		( *it )->vertex.deallocate( bufferOffset.vtxChunk );
	}

	VertexBufferPool::BufferArray::iterator VertexBufferPool::doFindBuffer( VkDeviceSize size
		, VertexBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end()
			&& !( *it )->vertex.hasAvailable( size ) )
		{
			++it;
		}

		return it;
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

	void ObjectBufferPool::upload( ashes::CommandBuffer const & commandBuffer )
	{
		for ( auto & buffer : m_buffers )
		{
			buffer->vertex.upload( commandBuffer );
			buffer->index.upload( commandBuffer );
		}
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto it = doFindBuffer( vertexCount, indexCount, m_buffers );

		if ( it == m_buffers.end() )
		{
			auto buffers = std::make_unique< ModelBuffers >( details::createBuffer< InterleavedVertex >( m_device
					, vertexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Vertex" + std::to_string( m_buffers.size() )
					, false )
				, details::createBuffer< uint32_t >( m_device
					, indexCount
					, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Index" + std::to_string( m_buffers.size() )
					, false ) );
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.vtxBuffer = &( *it )->vertex;
		result.vtxChunk = ( *it )->vertex.allocate( sizeof( InterleavedVertex ) * vertexCount );

		if ( indexCount )
		{
			result.idxBuffer = &( *it )->index;
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

	void SkinnedObjectBufferPool::upload( ashes::CommandBuffer const & commandBuffer )
	{
		for ( auto & buffer : m_buffers )
		{
			buffer->vertex.upload( commandBuffer );
			buffer->bones.upload( commandBuffer );
			buffer->index.upload( commandBuffer );
		}
	}

	ObjectBufferOffset SkinnedObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto it = doFindBuffer( vertexCount, indexCount, m_buffers );

		if ( it == m_buffers.end() )
		{
			auto buffers = std::make_unique< ModelBuffers >( details::createBuffer< InterleavedVertex >( m_device
					, vertexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Vertex" + std::to_string( m_buffers.size() )
					, false )
				, details::createBuffer< VertexBoneData >( m_device
					, indexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Bones" + std::to_string( m_buffers.size() )
					, false )
				, details::createBuffer< uint32_t >( m_device
					, indexCount
					, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + "Index" + std::to_string( m_buffers.size() )
					, false ) );
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.vtxBuffer = &( *it )->vertex;
		result.vtxChunk = ( *it )->vertex.allocate( sizeof( InterleavedVertex ) * vertexCount );
		result.bonBuffer = &( *it )->bones;
		result.bonChunk = ( *it )->bones.allocate( sizeof( VertexBoneData ) * vertexCount );

		if ( indexCount )
		{
			result.idxBuffer = &( *it )->index;
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
