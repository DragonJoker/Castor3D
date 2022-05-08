#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementCUSmartPtr( castor3d, VertexBufferPool )
CU_ImplementCUSmartPtr( castor3d, ObjectBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	namespace objbuf
	{
		static castor::String getName( SubmeshFlags submeshFlags )
		{
			castor::String result;

			if ( checkFlag( submeshFlags, SubmeshFlag::eSkinning ) )
			{
				result += "Skin";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphing ) )
			{
				result += "Morph";
			}

			return result;
		}
	}

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
			buffer.vertex->upload( commandBuffer );
		}
	}

	void VertexBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&bufferOffset]( ModelBuffers const & lookup )
			{
				return &lookup.vertex->getBuffer().getBuffer() == &bufferOffset.getVertexBuffer();
			} );
		CU_Require( it != m_buffers.end() );
		it->vertex->deallocate( bufferOffset.vtxChunk );
	}

	VertexBufferPool::BufferArray::iterator VertexBufferPool::doFindBuffer( VkDeviceSize size
		, VertexBufferPool::BufferArray & array )
	{
		CU_Require( size <= 65536u );
		auto it = array.begin();

		while ( it != array.end()
			&& !it->vertex->hasAvailable( size ) )
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
		for ( auto & bufferIt : m_buffers )
		{
			for ( auto & buffer : bufferIt.second )
			{
				buffer.vertex->upload( commandBuffer );

				if ( buffer.bones )
				{
					buffer.bones->upload( commandBuffer );
				}

				if ( buffer.morph )
				{
					buffer.morph->upload( commandBuffer );
				}

				if ( buffer.index )
				{
					buffer.index->upload( commandBuffer );
				}
			}
		}
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, SubmeshFlags submeshFlags )
	{
		ObjectBufferOffset result{ submeshFlags };
		auto & buffers = m_buffers.emplace( submeshFlags.value(), BufferArray{} ).first->second;
		auto it = doFindBuffer( vertexCount
			, indexCount
			, buffers );

		if ( it == buffers.end() )
		{
			auto name = objbuf::getName( submeshFlags );
			ModelBuffers modelBuffers{ details::createBuffer< InterleavedVertex >( m_device
					, vertexCount
					, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + name + "Vertex" + std::to_string( buffers.size() )
					, false )
				, details::createBuffer< uint32_t >( m_device
					, indexCount
					, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
					, m_debugName + name + "Index" + std::to_string( buffers.size() )
					, false )
				, ( checkFlag( submeshFlags, SubmeshFlag::eSkinning )
					? details::createBuffer< VertexBoneData >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Bones" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eMorphing )
					? details::createBuffer< InterleavedVertex >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Morph" + std::to_string( buffers.size() )
						, false )
					: nullptr ) };
			buffers.emplace_back( std::move( modelBuffers ) );
			it = std::next( buffers.begin()
				, ptrdiff_t( buffers.size() - 1u ) );
		}

		result.vtxBuffer = it->vertex.get();
		result.vtxChunk = it->vertex->allocate( sizeof( InterleavedVertex ) * vertexCount );

		if ( checkFlag( submeshFlags, SubmeshFlag::eSkinning ) )
		{
			result.bonBuffer = it->bones.get();
			result.bonChunk = it->bones->allocate( sizeof( VertexBoneData ) * vertexCount );
		}

		if ( checkFlag( submeshFlags, SubmeshFlag::eMorphing ) )
		{
			result.mphBuffer = it->morph.get();
			result.mphChunk = it->morph->allocate( sizeof( InterleavedVertex ) * vertexCount );
		}

		if ( indexCount )
		{
			result.idxBuffer = it->index.get();
			result.idxChunk = it->index->allocate( sizeof( uint32_t ) * indexCount );
		}

		return result;
	}

	void ObjectBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto buffersIt = m_buffers.find( bufferOffset.submeshFlags.value() );
		CU_Require( buffersIt  != m_buffers.end() );
		auto & buffers = buffersIt->second;
		auto it = std::find_if( buffers.begin()
			, buffers.end()
			, [&bufferOffset]( ModelBuffers const & lookup )
			{
				bool result = &lookup.vertex->getBuffer().getBuffer() == &bufferOffset.getVertexBuffer();

				if ( result && bufferOffset.bonBuffer )
				{
					result = &lookup.bones->getBuffer().getBuffer() == &bufferOffset.getBonesBuffer();
				}

				if ( result && bufferOffset.mphBuffer )
				{
					result = &lookup.morph->getBuffer().getBuffer() == &bufferOffset.getMorphBuffer();
				}

				if ( result && bufferOffset.idxBuffer )
				{
					result = &lookup.index->getBuffer().getBuffer() == &bufferOffset.getIndexBuffer();
				}

				return result;
			} );
		CU_Require( it != buffers.end() );
		it->vertex->deallocate( bufferOffset.vtxChunk );

		if ( bufferOffset.bonBuffer )
		{
			CU_Require( it->bones );
			it->bones->deallocate( bufferOffset.bonChunk );
		}

		if ( bufferOffset.mphBuffer )
		{
			CU_Require( it->morph );
			it->morph->deallocate( bufferOffset.mphChunk );
		}

		if ( bufferOffset.idxBuffer )
		{
			CU_Require( it->index );
			it->index->deallocate( bufferOffset.idxChunk );
		}
	}

	ObjectBufferPool::BufferArray::iterator ObjectBufferPool::doFindBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, ObjectBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end()
			&& ( !it->vertex->hasAvailable( sizeof( InterleavedVertex ) * vertexCount )
				|| !it->index->hasAvailable( sizeof( uint32_t ) * indexCount )
				|| ( it->bones && !it->bones->hasAvailable( sizeof( VertexBoneData ) * vertexCount ) )
				|| ( it->morph && !it->morph->hasAvailable( sizeof( InterleavedVertex ) * vertexCount ) ) ) )
		{
			++it;
		}

		return it;
	}

	//*********************************************************************************************
}
