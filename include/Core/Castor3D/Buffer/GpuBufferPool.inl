/*
See LICENSE file in root folder
*/
#include "Castor3D/Buffer/GpuBufferPool.hpp"

namespace c3d
{
	template< typename DataT >
	GpuBufferOffsetT< DataT > GpuBufferPool::getBuffer( BufferUsageFlags target
		, DeviceSize count
		, MemoryPropertyFlags flags )
	{
		MemChunk chunk{};
		auto & pool = doGetBuffer( count * sizeof( DataT ), target, flags, chunk );
		return GpuBufferOffsetT< DataT >{ pool
			, Buffer{ m_device, m_resources, chunk.bufferViewId }
			, target, flags
			, std::move( chunk ) };
	}

	template< typename DataT >
	void GpuBufferPool::putBuffer( GpuBufferOffsetT< DataT > const & bufferOffset )noexcept
	{
		doPutBuffer( bufferOffset.getPool()
			, bufferOffset.target
			, bufferOffset.memory
			, bufferOffset.chunk );
	}
}
