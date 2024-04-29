#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Render/RenderDevice.hpp"

namespace castor3d
{
	//*********************************************************************************************

	namespace details
	{
		template< typename DataT >
		GpuPackedBufferUPtr createBuffer( RenderDevice const & device
			, VkDeviceSize count
			, VkBufferUsageFlags usage
			, castor::String debugName
			, bool smallData
			, uint32_t alignSize )
		{
			VkDeviceSize maxCount = BaseObjectPoolBufferCount;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			return castor::makeUnique< GpuPackedBuffer >( device.renderSystem
				, usage
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ), alignSize }
				, smallData );
		}

		template< typename DataT >
		GpuPackedBaseBufferUPtr createBaseBuffer( RenderDevice const & device
			, VkDeviceSize count
			, VkBufferUsageFlags usage
			, VkMemoryPropertyFlags memory
			, castor::String debugName
			, uint32_t alignSize )
		{
			VkDeviceSize maxCount = BaseObjectPoolBufferCount;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			return castor::makeUnique< GpuPackedBaseBuffer >( device
				, usage
				, memory
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ), alignSize } );
		}
	}

	//*********************************************************************************************

	template< typename VertexT >
	ObjectBufferOffset VertexBufferPool::getBuffer( VkDeviceSize vertexCount )
	{
		ObjectBufferOffset result;
		auto size = VkDeviceSize( vertexCount * sizeof( VertexT ) );
		auto it = doFindBuffer( size, m_buffers );

		if ( it == m_buffers.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, std::max( size, 65536ULL )
				, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, m_debugName + cuT( "Vertex" ) + castor::string::toString( m_buffers.size() )
				, uint32_t( m_device.properties.limits.minMemoryMapAlignment ) ) };
			m_buffers.emplace_back( castor::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.buffers[uint32_t( SubmeshData::ePositions )].buffer = it->vertex.get();
		result.buffers[uint32_t( SubmeshData::ePositions )].chunk = it->vertex->allocate( size );
		return result;
	}

	//*********************************************************************************************

	template< typename IndexT >
	ObjectBufferOffset IndexBufferPool::getBuffer( VkDeviceSize indexCount )
	{
		ObjectBufferOffset result;
		auto size = VkDeviceSize( indexCount * sizeof( IndexT ) );
		auto it = doFindBuffer( size, m_buffers );

		if ( it == m_buffers.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, std::max( size, 65536ULL )
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, m_debugName + cuT( "Index" ) + castor::string::toString( m_buffers.size() )
				, uint32_t( m_device.properties.limits.minMemoryMapAlignment ) ) };
			m_buffers.emplace_back( castor::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.buffers[uint32_t( SubmeshData::eIndex )].buffer = it->index.get();
		result.buffers[uint32_t( SubmeshData::eIndex )].chunk = it->index->allocate( size );
		return result;
	}

	//*********************************************************************************************
}
