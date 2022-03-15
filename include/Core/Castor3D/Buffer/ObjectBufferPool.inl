#include "Castor3D/Buffer/ObjectBufferPool.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

namespace castor3d
{
	//*********************************************************************************************

	namespace details
	{
		template< typename DataT >
		GpuPackedBuffer createBuffer( RenderDevice const & device
			, VkDeviceSize count
			, VkBufferUsageFlags usage
			, std::string debugName
			, bool smallData )
		{
			VkDeviceSize maxCount = 1'000'000;

			while ( maxCount < count )
			{
				maxCount *= 2u;
			}

			return GpuPackedBuffer{ device.renderSystem
				, usage
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, debugName
				, ashes::QueueShare{}
				, GpuBufferPackedAllocator{ uint32_t( maxCount * sizeof( DataT ) ) }
				, smallData };
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
			auto buffers = std::make_unique< ModelBuffers >( details::createBuffer< uint8_t >( m_device
				, size
				, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, m_debugName + "Vertex" + std::to_string( m_buffers.size() )
				, true ) );
			m_buffers.emplace_back( std::move( buffers ) );
			it = std::next( m_buffers.begin()
				, ptrdiff_t( m_buffers.size() - 1u ) );
		}

		result.vtxBuffer = &( *it )->vertex;
		result.vtxChunk = ( *it )->vertex.allocate( size );
		return result;
	}

	//*********************************************************************************************
}
