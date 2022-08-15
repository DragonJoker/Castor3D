#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

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
		static castor::String getName( SubmeshFlags submeshFlags
			, bool isGpuComputed )
		{
			castor::String result;

			if ( isGpuComputed )
			{
				result += "G";
			}
			
			if ( checkFlag( submeshFlags, SubmeshFlag::eIndex ) )
			{
				result += "I";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
			{
				result += "P";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
			{
				result += "N";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
			{
				result += "T";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
			{
				result += "T0";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
			{
				result += "1";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
			{
				result += "2";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
			{
				result += "3";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
			{
				result += "C";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eSkin ) )
			{
				result += "S";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::ePassMasks ) )
			{
				result += "M";
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
				return &lookup.vertex->getBuffer().getBuffer() == &bufferOffset.getBuffer( SubmeshFlag::ePositions );
			} );
		CU_Require( it != m_buffers.end() );
		it->vertex->deallocate( bufferOffset.buffers[getIndex( SubmeshFlag::ePositions )].chunk );
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
			for ( auto & buffers : bufferIt.second )
			{
				for ( uint32_t i = 0u; i < uint32_t( SubmeshData::eCount ); ++i )
				{
					if ( buffers.buffers[i] )
					{
						buffers.buffers[i]->upload( commandBuffer );
					}
				}
			}
		}
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, SubmeshFlags submeshFlags )
	{
		auto result = doGetBuffer( vertexCount, indexCount, submeshFlags, false );

		if ( indexCount && vertexCount )
		{
			m_indexBuffers.emplace( &result.getBuffer( SubmeshFlag::ePositions )
				, &result.getBuffer( SubmeshFlag::eIndex ) );
		}

		return result;
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, ashes::BufferBase const * indexBuffer
		, SubmeshFlags submeshFlags )
	{
		auto result = doGetBuffer( vertexCount, 0u, submeshFlags, true );

		if ( indexBuffer )
		{
			m_indexBuffers.emplace( &result.getBuffer( SubmeshFlag::ePositions )
				, indexBuffer );
		}

		return result;
	}

	ObjectBufferPool::ModelBuffers const & ObjectBufferPool::getBuffers( ashes::BufferBase const & buffer )
	{
		ObjectBufferPool::ModelBuffers const * result{};
		auto mit = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&buffer, &result]( std::unordered_map< size_t, BufferArray >::value_type const & mapLookup )
			{
				auto bit = std::find_if( mapLookup.second.begin()
					, mapLookup.second.end()
					, [&buffer]( ModelBuffers const & lookup )
					{
						return &lookup.buffers[size_t( SubmeshData::ePositions )]->getBuffer().getBuffer() == &buffer;
					} );

				if ( bit != mapLookup.second.end() )
				{
					result = &( *bit );
				}

				return result != nullptr;
			} );

		if ( mit == m_buffers.end() )
		{
			CU_Exception( "Couldn't find buffer in pool" );
		}

		return *result;
	}

	ashes::BufferBase const & ObjectBufferPool::getIndexBuffer( ashes::BufferBase const & buffer )
	{
		auto it = m_indexBuffers.find( &buffer );

		if ( it == m_indexBuffers.end() )
		{
			CU_Exception( "Couldn't find the index buffer linked to positions buffer." );
		}

		return *it->second;
	}

	void ObjectBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )
	{
		auto buffersIt = m_buffers.find( bufferOffset.hash );
		CU_Require( buffersIt  != m_buffers.end() );
		auto & buffers = buffersIt->second;
		auto it = std::find_if( buffers.begin()
			, buffers.end()
			, [&bufferOffset]( ModelBuffers const & lookup )
			{
				bool result = true;

				for ( uint32_t i = 0u; i < uint32_t( SubmeshData::eCount ); ++i )
				{
					if ( result && lookup.buffers[i] && bufferOffset.buffers[i].buffer )
					{
						result = &lookup.buffers[i]->getBuffer().getBuffer() == &bufferOffset.buffers[i].getBuffer();
					}
				}

				return result;
			} );
		CU_Require( it != buffers.end() );

		for ( uint32_t i = 0u; i < uint32_t( SubmeshData::eCount ); ++i )
		{
			if ( bufferOffset.buffers[i].buffer )
			{
				CU_Require( it->buffers[i] );
				it->buffers[i]->deallocate( bufferOffset.buffers[i].chunk );
			}
		}
	}

	ObjectBufferOffset ObjectBufferPool::doGetBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, SubmeshFlags submeshFlags
		, bool isGpuComputed )
	{
		auto hash = std::hash< SubmeshFlags::BaseType >{}( submeshFlags.value() );
		hash = castor::hashCombine( hash, indexCount != 0u );
		hash = castor::hashCombine( hash, vertexCount != 0u );
		hash = castor::hashCombine( hash, isGpuComputed );
		ObjectBufferOffset result{ hash };
		auto & buffers = m_buffers.emplace( hash, BufferArray{} ).first->second;
		auto it = doFindBuffer( vertexCount
			, indexCount
			, buffers );
		auto align = uint32_t( m_device.properties.limits.nonCoherentAtomSize );

		if ( it == buffers.end() )
		{
			auto name = objbuf::getName( submeshFlags, isGpuComputed );
			ModelBuffers modelBuffers;

			for ( uint32_t i = 0u; i < uint32_t( SubmeshData::eCount ); ++i )
			{
				if ( !checkFlag( submeshFlags, SubmeshFlag( 0x0001u << i ) ) )
				{
					continue;
				}

				auto data = SubmeshData( i );

				if ( data == SubmeshData::eIndex )
				{
					if ( indexCount )
					{
						modelBuffers.buffers[i] = details::createBuffer< uint32_t >( m_device
							, indexCount
							, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, m_debugName + name + getName( data ) + std::to_string( buffers.size() )
							, false
							, align );
					}
				}
				else if ( vertexCount )
				{
					switch ( data )
					{
					case SubmeshData::eSkin:
						modelBuffers.buffers[i] = details::createBuffer< VertexBoneData >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, m_debugName + name + getName( data ) + std::to_string( buffers.size() )
							, false
							, align );
						break;
					case SubmeshData::ePassMasks:
						modelBuffers.buffers[i] = details::createBuffer< castor::Point4ui >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, m_debugName + name + getName( data ) + std::to_string( buffers.size() )
							, false
							, align );
						break;
					default:
						modelBuffers.buffers[i] = details::createBuffer< castor::Point4f >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, m_debugName + name + getName( data ) + std::to_string( buffers.size() )
							, false
							, align );
						break;
					}
				}
			}

			buffers.emplace_back( std::move( modelBuffers ) );
			it = std::next( buffers.begin()
				, ptrdiff_t( buffers.size() - 1u ) );
		}

		if ( indexCount )
		{
			static_assert( uint32_t( SubmeshData::eIndex ) == 0u );
			result.buffers[0u].buffer = it->buffers[0u].get();
			result.buffers[0u].chunk = it->buffers[0u]->allocate( getSize( SubmeshData::eIndex ) * indexCount );
		}

		if ( vertexCount )
		{
			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eCount ); ++i )
			{
				if ( it->buffers[i] )
				{
					result.buffers[i].buffer = it->buffers[i].get();
					result.buffers[i].chunk = it->buffers[i]->allocate( getSize( SubmeshData( i ) ) * vertexCount );
				}
			}
		}

		return result;
	}

	ObjectBufferPool::BufferArray::iterator ObjectBufferPool::doFindBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, ObjectBufferPool::BufferArray & array )
	{
		return std::find_if( array.begin()
			, array.end()
			, [vertexCount, indexCount]( ModelBuffers const & lookup )
			{
				auto it = lookup.buffers.begin();
				uint32_t index = 0u;

				if ( ( *it )
					&& !( *it )->hasAvailable( getSize( SubmeshData( index++ ) ) * indexCount ) )
				{
					return false;
				}

				++it;
				return lookup.buffers.end() == std::find_if( it
					, lookup.buffers.end()
					, [vertexCount, &index]( GpuPackedBufferPtr const & buffer )
					{
						return buffer
							&& !buffer->hasAvailable( getSize( SubmeshData( index++ ) ) * vertexCount );
					} );
			} );
	}

	//*********************************************************************************************
}
