/*
See LICENSE file in root folder
*/
#include "Castor3D/Buffer/GpuBufferPool.hpp"

namespace castor3d
{
	template< typename DataT >
	GpuBufferOffsetT< DataT > GpuBufferPool::getBuffer( VkBufferUsageFlagBits target
		, VkDeviceSize count
		, VkMemoryPropertyFlags flags )
	{
		GpuBufferOffsetT< DataT > result;
		result.target = target;
		result.memory = flags;
		result.setPool( &doGetBuffer( count * sizeof( DataT )
			, result.target
			, result.memory
			, result.chunk ) );
		return result;
	}

	template< typename DataT >
	void GpuBufferPool::putBuffer( GpuBufferOffsetT< DataT > const & bufferOffset )
	{
		doPutBuffer( *bufferOffset.buffer
			, bufferOffset.target
			, bufferOffset.memory
			, bufferOffset.chunk );
	}
}
