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
	public:
		GpuBufferBase * buffer{};
		VkBufferUsageFlags target{};
		VkMemoryPropertyFlags memory{};
		MemChunk chunk{};

		void setPool( GpuBufferBase & pool )
		{
			buffer = &pool;
		}

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		GpuBufferBase const & getPool()const
		{
			return *buffer;
		}

		GpuBufferBase & getPool()
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

		VkDeviceSize getCount()const
		{
			return chunk.askedSize / sizeof( DataT );
		}

		VkDeviceSize getSize()const
		{
			return chunk.size;
		}

		VkDeviceSize getOffset()const
		{
			return chunk.offset;
		}

		castor::ArrayView< DataT > getData()
		{
			return castor::makeArrayView( reinterpret_cast< DataT * >( buffer->getDatas().data() + getOffset() )
				, getCount() );
		}
	};
}

#endif
