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
			, std::string debugName
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
			, std::string debugName
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
		auto size = vertexCount * sizeof( VertexT );
		auto it = doFindBuffer( size, m_buffers );

		if ( it == m_buffers.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, size
				, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, m_debugName + "Vertex" + std::to_string( m_buffers.size() )
				, 1u ) };
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.buffers[getIndex( SubmeshFlag::ePositions )].buffer = it->vertex.get();
		result.buffers[getIndex( SubmeshFlag::ePositions )].chunk = it->vertex->allocate( size );
		return result;
	}

	//*********************************************************************************************

	template< typename IndexT >
	ObjectBufferOffset IndexBufferPool::getBuffer( VkDeviceSize vertexCount )
	{
		ObjectBufferOffset result;
		auto size = vertexCount * sizeof(IndexT);
		auto it = doFindBuffer( size, m_buffers );

		if ( it == m_buffers.end() )
		{
			ModelBuffers buffers{ details::createBaseBuffer< uint8_t >( m_device
				, size
				, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, m_debugName + "Index" + std::to_string( m_buffers.size() )
				, 1u ) };
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.buffers[getIndex( SubmeshFlag::eIndex )].buffer = it->vertex.get();
		result.buffers[getIndex( SubmeshFlag::eIndex )].chunk = it->vertex->allocate( size );
		return result;
	}

	//*********************************************************************************************
}
