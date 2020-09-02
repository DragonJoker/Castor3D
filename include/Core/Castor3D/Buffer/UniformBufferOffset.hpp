/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferOffset_HPP___
#define ___C3D_UniformBufferOffset_HPP___

#include "BufferModule.hpp"

namespace castor3d
{
	template< typename DataT, typename BufferT >
	struct UniformBufferOffsetT
	{
		BufferT * buffer{ nullptr };
		VkMemoryPropertyFlags flags{ 0u };
		uint32_t offset{ 0u };

		explicit operator bool()const
		{
			return buffer
				&& buffer->hasBuffer();
		}

		DataT const & getData()const
		{
			return buffer->getData( offset );
		}

		DataT & getData()
		{
			return buffer->getData( offset );
		}

		uint32_t getAlignedSize()const
		{
			return buffer->getAlignedSize();
		}
	};
}

#endif
