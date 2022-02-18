/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferOffset_HPP___
#define ___C3D_ObjectBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Model/VertexGroup.hpp"

namespace castor3d
{
	struct ObjectBufferOffset
	{
	public:
		ashes::BufferBase const * vtxBuffer{};
		ashes::BufferBase const * idxBuffer{};
		MemChunk vtxChunk{};
		MemChunk idxChunk{};

		explicit operator bool()const
		{
			return vtxBuffer != nullptr;
		}

		ashes::BufferBase const & getIndexBuffer()const
		{
			return *idxBuffer;
		}

		ashes::BufferBase const & getVertexBuffer()const
		{
			return *vtxBuffer;
		}

		uint32_t getIndexCount()const
		{
			return uint32_t( idxChunk.askedSize / sizeof( uint32_t ) );
		}

		uint32_t getVertexCount()const
		{
			return uint32_t( vtxChunk.askedSize / sizeof( InterleavedVertex ) );
		}

		VkDeviceSize getIndexOffset()const
		{
			return idxChunk.offset;
		}

		VkDeviceSize getVertexOffset()const
		{
			return vtxChunk.offset;
		}

		uint32_t getFirstIndex()const
		{
			return uint32_t( getIndexOffset() / sizeof( uint32_t ) );
		}

		uint32_t getFirstVertex()const
		{
			return uint32_t( getVertexOffset() / sizeof( InterleavedVertex ) );
		}
	};
}

#endif
