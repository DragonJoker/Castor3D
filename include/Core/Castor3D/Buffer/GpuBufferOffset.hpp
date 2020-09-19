/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferOffset_HPP___
#define ___C3D_GpuBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"

namespace castor3d
{
	template< typename DataT >
	struct GpuBufferOffsetT
	{
	private:
		GpuBuffer const * buffer;

	public:
		VkBufferUsageFlagBits target;
		VkMemoryPropertyFlags memory;
		MemChunk chunk;

		void setPool( GpuBuffer & pool )
		{
			buffer = &pool;
		}

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		GpuBuffer const & getPool()const
		{
			return *buffer;
		}

		GpuBuffer & getPool()
		{
			return *buffer;
		}

		ashes::Buffer< uint8_t > const & getBuffer()const
		{
			return buffer->getBuffer();
		}

		ashes::Buffer< uint8_t > & getBuffer()
		{
			return buffer->getBuffer();
		}
	};
}

#endif
