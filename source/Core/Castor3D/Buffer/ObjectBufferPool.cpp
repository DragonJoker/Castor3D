#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

#include <numeric>

CU_ImplementSmartPtr( castor3d, IndexBufferPool )
CU_ImplementSmartPtr( castor3d, VertexBufferPool )
CU_ImplementSmartPtr( castor3d, ObjectBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	namespace objbuf
	{
		static castor::String getName( SubmeshComponentCombine const & components
			, bool isGpuComputed )
		{
			castor::String result;

			if ( isGpuComputed )
			{
				result += cuT( "G" );
			}
			
			if ( components.hasTriangleIndexFlag
				|| components.hasLineIndexFlag )
			{
				result += cuT( "I" );
			}

			if ( components.hasPositionFlag )
			{
				result += cuT( "P" );
			}

			if ( components.hasTangentFlag )
			{
				result += cuT( "T" );
			}

			if ( components.hasBitangentFlag )
			{
				result += cuT( "B" );
			}

			if ( components.hasNormalFlag )
			{
				result += cuT( "N" );
			}

			if ( components.hasTexcoord0Flag )
			{
				result += cuT( "T0" );
			}

			if ( components.hasTexcoord1Flag )
			{
				result += cuT( "T1" );
			}

			if ( components.hasTexcoord2Flag )
			{
				result += cuT( "T2" );
			}

			if ( components.hasTexcoord3Flag )
			{
				result += cuT( "T3" );
			}

			if ( components.hasColourFlag )
			{
				result += cuT( "C" );
			}

			if ( components.hasSkinFlag )
			{
				result += cuT( "S" );
			}

			if ( components.hasPassMaskFlag )
			{
				result += cuT( "M" );
			}

			return result;
		}
	}

	//*********************************************************************************************

	VertexBufferPool::VertexBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ castor::move( debugName ) }
	{
	}

	void VertexBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	castor::Vector< castor::Pair< size_t, VertexBufferPool::BufferArray > >::iterator VertexBufferPool::doInsertBuffers( size_t align )
	{
		align = std::lcm( align, m_device.properties.limits.minMemoryMapAlignment );
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [align]( castor::Pair< size_t, VertexBufferPool::BufferArray > const & lookup )
			{
				return align == lookup.first;
			} );

		if ( it == m_buffers.end() )
		{
			m_buffers.emplace_back( align, BufferArray{} );
			it = std::next( m_buffers.begin(), ptrdiff_t( m_buffers.size() - 1u ) );
		}

		return it;
	}

	castor::Vector< castor::Pair< size_t, VertexBufferPool::BufferArray > >::const_iterator VertexBufferPool::doFindBuffers( size_t align )const
	{
		align = std::lcm( align, m_device.properties.limits.minMemoryMapAlignment );
		return std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [align]( castor::Pair< size_t, VertexBufferPool::BufferArray > const & lookup )
			{
				return align == lookup.first;
			} );
	}

	VertexBufferPool::BufferArray::iterator VertexBufferPool::doFindBuffer( VkDeviceSize size
		, VertexBufferPool::BufferArray & array )const
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

	IndexBufferPool::IndexBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_device{ device }
		, m_debugName{ castor::move( debugName ) }
	{
	}

	void IndexBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	void IndexBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )noexcept
	{
		auto it = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&bufferOffset]( ModelBuffers const & lookup )
			{
				return &lookup.index->getBuffer() == &bufferOffset.getBuffer( SubmeshData::eIndex );
			} );
		CU_Require( it != m_buffers.end() );
		it->index->deallocate( bufferOffset.buffers[uint32_t( SubmeshData::eIndex )].chunk );
	}

	IndexBufferPool::BufferArray::iterator IndexBufferPool::doFindBuffer( VkDeviceSize size
		, IndexBufferPool::BufferArray & array )const
	{
		CU_Require( size <= 65536u );
		auto it = array.begin();

		while ( it != array.end()
			&& !it->index->hasAvailable( size ) )
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
		, m_debugName{ castor::move( debugName ) }
	{
	}

	void ObjectBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, VkDeviceSize meshletCount
		, SubmeshComponentCombine const & components )
	{
		auto result = doGetBuffer( vertexCount, indexCount, meshletCount, components, false );

		if ( indexCount && vertexCount )
		{
			m_indexBuffers.emplace( &result.getBuffer( SubmeshData::ePositions )
				, &result.getBuffer( SubmeshData::eIndex ) );
		}

		return result;
	}

	ObjectBufferOffset ObjectBufferPool::getBuffer( VkDeviceSize vertexCount
		, ashes::BufferBase const * indexBuffer
		, SubmeshComponentCombine const & components )
	{
		auto result = doGetBuffer( vertexCount, 0u, 0u, components, true );

		if ( indexBuffer )
		{
			m_indexBuffers.emplace( &result.getBuffer( SubmeshData::ePositions )
				, indexBuffer );
		}

		return result;
	}

	ObjectBufferPool::ModelBuffers const & ObjectBufferPool::getBuffers( ashes::BufferBase const & buffer )
	{
		ObjectBufferPool::ModelBuffers const * result{};

		if ( auto mit = std::find_if( m_buffers.begin()
			, m_buffers.end()
			, [&buffer, &result]( castor::UnorderedMap< size_t, BufferArray >::value_type const & mapLookup )
			{
				if ( auto bit = std::find_if( mapLookup.second.begin()
					, mapLookup.second.end()
					, [&buffer]( ModelBuffers const & lookup )
					{
						return lookup.buffers[size_t( SubmeshData::ePositions )]
							&& &lookup.buffers[size_t( SubmeshData::ePositions )]->getBuffer() == &buffer;
					} );
					bit != mapLookup.second.end() )
				{
					result = &( *bit );
				}

				return result != nullptr;
			} );
			mit == m_buffers.end() )
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

	void ObjectBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )noexcept
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
						result = &lookup.buffers[i]->getBuffer() == &bufferOffset.buffers[i].getBuffer();
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
		, VkDeviceSize meshletCount
		, SubmeshComponentCombine const & components
		, bool isGpuComputed )
	{
		auto hash = std::hash< SubmeshComponentCombineID >{}( components.baseId );
		hash = castor::hashCombine( hash, indexCount != 0u );
		hash = castor::hashCombine( hash, vertexCount != 0u );
		hash = castor::hashCombine( hash, isGpuComputed );
		ObjectBufferOffset result{ hash };
		auto & buffers = m_buffers.try_emplace( hash ).first->second;
		auto it = doFindBuffer( vertexCount
			, indexCount
			, meshletCount
			, buffers );
		auto align = uint32_t( m_device.properties.limits.nonCoherentAtomSize );

		if ( it == buffers.end() )
		{
			auto & submeshComponents = getOwner()->getEngine()->getSubmeshComponentsRegister();
			auto name = objbuf::getName( components, isGpuComputed );
			ModelBuffers modelBuffers;

			for ( auto & flag : components.flags )
			{
				auto data = submeshComponents.getSubmeshData( flag );
				auto index = uint32_t( data );

				if ( data == SubmeshData::eIndex )
				{
					if ( indexCount )
					{
						modelBuffers.buffers[index] = details::createBaseBuffer< uint32_t >( m_device
							, indexCount
							, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, m_debugName + name + getName( data ) + castor::string::toString( buffers.size() )
							, align );
					}
				}
				else if ( data == SubmeshData::eMeshlets )
				{
					if ( meshletCount )
					{
						modelBuffers.buffers[index] = details::createBaseBuffer< Meshlet >( m_device
							, meshletCount
							, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, m_debugName + name + getName( data ) + castor::string::toString( buffers.size() )
							, align );
					}
				}
				else if ( data < SubmeshData::eCount && vertexCount )
				{
					switch ( data )
					{
					case SubmeshData::eSkin:
						modelBuffers.buffers[index] = details::createBaseBuffer< VertexBoneData >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, m_debugName + name + getName( data ) + castor::string::toString( buffers.size() )
							, align );
						break;
					case SubmeshData::ePassMasks:
						modelBuffers.buffers[index] = details::createBaseBuffer< castor::Point4ui >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, m_debugName + name + getName( data ) + castor::string::toString( buffers.size() )
							, align );
						break;
					default:
						modelBuffers.buffers[index] = details::createBaseBuffer< castor::Point4f >( m_device
							, vertexCount
							, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, m_debugName + name + getName( data ) + castor::string::toString( buffers.size() )
							, align );
						break;
					}
				}
			}

			buffers.emplace_back( castor::move( modelBuffers ) );
			it = std::next( buffers.begin()
				, ptrdiff_t( buffers.size() - 1u ) );
		}

		if ( indexCount )
		{
			static_assert( uint32_t( SubmeshData::eIndex ) == 0u );
			result.buffers[0u].buffer = it->buffers[0u].get();
			result.buffers[0u].chunk = it->buffers[0u]->allocate( getSize( SubmeshData::eIndex ) * indexCount );
		}

		if ( meshletCount )
		{
			auto index = uint32_t( SubmeshData::eMeshlets );
			result.buffers[index].buffer = it->buffers[index].get();
			result.buffers[index].chunk = it->buffers[index]->allocate( getSize( SubmeshData::eMeshlets ) * meshletCount );
		}

		if ( vertexCount )
		{
			for ( uint32_t i = 1u; i < uint32_t( SubmeshData::eMeshlets ); ++i )
			{
				if ( it->buffers[i] )
				{
					result.buffers[i].buffer = it->buffers[i].get();
					result.buffers[i].chunk = it->buffers[i]->allocate( getSize( SubmeshData( i ) ) * vertexCount );
				}
			}
		}

		result.id = uint16_t( std::distance( buffers.begin(), it ) );
		return result;
	}

	ObjectBufferPool::BufferArray::iterator ObjectBufferPool::doFindBuffer( VkDeviceSize vertexCount
		, VkDeviceSize indexCount
		, VkDeviceSize meshletCount
		, ObjectBufferPool::BufferArray & array )const
	{
		return std::find_if( array.begin()
			, array.end()
			, [vertexCount, indexCount, meshletCount]( ModelBuffers const & lookup )
			{
				uint32_t index{};
				return lookup.buffers.end() == std::find_if( lookup.buffers.begin()
					, lookup.buffers.end()
					, [vertexCount, indexCount, meshletCount, &index]( GpuPackedBaseBufferUPtr const & buffer )
					{
						if ( index == uint32_t( SubmeshData::eIndex ) )
						{
							return buffer
								&& !buffer->hasAvailable( getSize( SubmeshData( index++ ) ) * indexCount );
						}

						if ( index == uint32_t( SubmeshData::eMeshlets ) )
						{
							return buffer
								&& !buffer->hasAvailable( getSize( SubmeshData( index++ ) ) * meshletCount );
						}

						return buffer
							&& !buffer->hasAvailable( getSize( SubmeshData( index++ ) ) * vertexCount );
					} );
			} );
	}

	//*********************************************************************************************
}
