#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

namespace c3d
{
	//*********************************************************************************************

	namespace details
	{
		template< typename DataT >
		GpuPackedBufferUPtr createBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, DeviceSize count
			, BufferUsageFlags usage
			, String debugName
			, bool smallData
			, uint32_t alignSize )
		{
			DeviceSize maxCount = BaseObjectPoolBufferCount;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			return makeUnique< GpuPackedBuffer >( device.renderSystem
				, resources
				, usage
				, MemoryPropertyFlags::eDeviceLocal
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ), alignSize }
				, smallData );
		}

		template< typename DataT >
		GpuPackedBaseBufferUPtr createBaseBuffer( RenderDevice const & device
			, crg::ResourcesCache & resources
			, DeviceSize count
			, BufferUsageFlags usage
			, MemoryPropertyFlags memory
			, String debugName
			, uint32_t alignSize )
		{
			DeviceSize maxCount = BaseObjectPoolBufferCount;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			return makeUnique< GpuPackedBaseBuffer >( device
				, resources
				, usage
				, memory
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ), alignSize } );
		}
	}

	//*********************************************************************************************

	template< typename VertexT >
	ObjectBufferOffset VertexBufferPool::getBuffer( DeviceSize vertexCount )
	{
		ObjectBufferOffset result;
		auto size = DeviceSize( vertexCount * sizeof( VertexT ) );
		auto ait = doInsertBuffers( sizeof( VertexT ) );
		auto it = doFindBuffer( size, ait->second );

		if ( it == ait->second.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, m_resources
				, std::max( size, DeviceSize( 65536U ) )
				, BufferUsageFlags::eVertexBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eHostVisible
				, m_debugName + cuT( "Vertex" ) + string::toString( ait->second.size() )
				, uint32_t( ait->first ) ) };
			ait->second.emplace_back( c3d::move( buffers ) );
			it = std::next( ait->second.begin()
				, ptrdiff_t( ait->second.size() - 1u ) );
		}

		result.buffers[uint32_t( SubmeshData::ePositions )].buffer = it->vertex.get();
		result.buffers[uint32_t( SubmeshData::ePositions )].chunk = it->vertex->allocate( size );
		return result;
	}

	template< typename VertexT >
	void VertexBufferPool::putBuffer( ObjectBufferOffset const & bufferOffset )noexcept
	{
		auto ait = doFindBuffers( sizeof( VertexT ) );
		CU_Require( ait != m_buffers.end() );
		auto it = std::find_if( ait->second.begin()
			, ait->second.end()
			, [&bufferOffset]( ModelBuffers const & lookup )
			{
				return &lookup.vertex->getBuffer() == &bufferOffset.getBuffer( SubmeshData::ePositions );
			} );
		CU_Require( it != ait->second.end() );
		it->vertex->deallocate( bufferOffset.buffers[uint32_t( SubmeshData::ePositions )].chunk );
	}

	//*********************************************************************************************

	template< typename IndexT >
	ObjectBufferOffset IndexBufferPool::getBuffer( DeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto size = DeviceSize( indexCount * sizeof( IndexT ) );
		auto it = doFindBuffer( size, m_buffers );

		if ( it == m_buffers.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, m_resources
				, std::max( size, DeviceSize( 65536U ) )
				, BufferUsageFlags::eIndexBuffer | BufferUsageFlags::eTransferDst
				, MemoryPropertyFlags::eHostVisible
				, m_debugName + cuT( "Index" ) + string::toString( m_buffers.size() )
				, uint32_t( m_device.properties.limits.minMemoryMapAlignment ) ) };
			m_buffers.emplace_back( c3d::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.buffers[uint32_t( SubmeshData::eIndex )].buffer = it->index.get();
		result.buffers[uint32_t( SubmeshData::eIndex )].chunk = it->index->allocate( size );
		return result;
	}

	//*********************************************************************************************
}
