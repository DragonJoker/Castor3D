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

			if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
			{
				result += "Pos";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
			{
				result += "Nml";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
			{
				result += "Tan";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords ) )
			{
				result += "Tex";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eSecondaryUV ) )
			{
				result += "SecUV";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eBones ) )
			{
				result += "Bones";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphPositions ) )
			{
				result += "MphPos";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphNormals ) )
			{
				result += "MphNml";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTangents ) )
			{
				result += "MphTan";
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords ) )
			{
				result += "MphTex";
			}

			return result;
		}

		static uint32_t getSize( uint32_t flagIndex )
		{
			constexpr size_t sizes[] = { sizeof( uint32_t ) /* SubmeshFlag::eIndex */
				, sizeof( castor::Point3f ) /* SubmeshFlag::ePositions */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eNormals */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eTangents */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphPositions */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphNormals */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTangents */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords */
				, sizeof( VertexBoneData ) /* SubmeshFlag::eBones */
				, sizeof( castor::Point3f ) /* SubmeshFlag::eSecondaryUV */ };
			return uint32_t( sizes[flagIndex] );
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
				for ( uint32_t i = 0u; i < getIndex( SubmeshFlag::eAllComponents ); ++i )
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
		ObjectBufferOffset result{ submeshFlags };
		auto & buffers = m_buffers.emplace( submeshFlags.value(), BufferArray{} ).first->second;
		auto it = doFindBuffer( vertexCount
			, indexCount
			, buffers );

		if ( it == buffers.end() )
		{
			auto name = objbuf::getName( submeshFlags );
			ModelBuffers modelBuffers{ { ( indexCount
					? details::createBuffer< uint32_t >( m_device
						, indexCount
						, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Index" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::ePositions )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Positions" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eNormals )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Normals" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTangents )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Tangents" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Texcoords" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eMorphPositions )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "MorphPos" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eMorphNormals )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "MorphNml" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eMorphTangents )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "MorphTan" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eMorphTexcoords )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "MorphTex" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eBones )
					? details::createBuffer< VertexBoneData >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "Bones" + std::to_string( buffers.size() )
						, false )
					: nullptr )
				, ( checkFlag( submeshFlags, SubmeshFlag::eSecondaryUV )
					? details::createBuffer< castor::Point3f >( m_device
						, vertexCount
						, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
						, m_debugName + name + "SecondaryUVs" + std::to_string( buffers.size() )
						, false )
					: nullptr ) } };
			buffers.emplace_back( std::move( modelBuffers ) );
			it = std::next( buffers.begin()
				, ptrdiff_t( buffers.size() - 1u ) );
		}

		if ( indexCount )
		{
			result.buffers[0u].buffer = it->buffers[0u].get();
			result.buffers[0u].chunk = it->buffers[0u]->allocate( objbuf::getSize( 0u ) * indexCount );
		}

		for ( uint32_t i = 1u; i < getIndex( SubmeshFlag::eAllComponents ); ++i )
		{
			if ( it->buffers[i] )
			{
				result.buffers[i].buffer = it->buffers[i].get();
				result.buffers[i].chunk = it->buffers[i]->allocate( objbuf::getSize( i ) * vertexCount );
			}
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
				bool result = true;

				for ( uint32_t i = 0u; i < getIndex( SubmeshFlag::eAllComponents ); ++i )
				{
					if ( result && lookup.buffers[i] && bufferOffset.buffers[i].buffer )
					{
						result = &lookup.buffers[i]->getBuffer().getBuffer() == &bufferOffset.buffers[i].getBuffer();
					}
				}

				return result;
			} );
		CU_Require( it != buffers.end() );

		for ( uint32_t i = 0u; i < getIndex( SubmeshFlag::eAllComponents ); ++i )
		{
			if ( bufferOffset.buffers[i].buffer )
			{
				CU_Require( it->buffers[i] );
				it->buffers[i]->deallocate( bufferOffset.buffers[i].chunk );
			}
		}
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
					&& !( *it )->hasAvailable( objbuf::getSize( index++ ) * indexCount ) )
				{
					return false;
				}

				++it;
				return lookup.buffers.end() == std::find_if( it
					, lookup.buffers.end()
					, [vertexCount, &index]( GpuPackedBufferPtr const & buffer )
					{
						return buffer
							&& !buffer->hasAvailable( objbuf::getSize( index++ ) * vertexCount );
					} );
			} );
	}

	//*********************************************************************************************
}
