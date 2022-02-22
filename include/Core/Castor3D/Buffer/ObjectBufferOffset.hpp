/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferOffset_HPP___
#define ___C3D_ObjectBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

namespace castor3d
{
	struct ObjectBufferOffset
	{
	public:
		ashes::BufferBase const * vtxBuffer{};
		ashes::BufferBase const * idxBuffer{};
		ashes::BufferBase const * bonBuffer{};
		MemChunk vtxChunk{};
		MemChunk idxChunk{};
		MemChunk bonChunk{};

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

		ashes::BufferBase const & getBonesBuffer()const
		{
			return *bonBuffer;
		}

		bool hasIndices()const
		{
			return idxChunk.askedSize != 0;
		}

		bool hasVertices()const
		{
			return vtxChunk.askedSize != 0;
		}

		bool hasBones()const
		{
			return bonChunk.askedSize != 0;
		}

		uint32_t getIndexCount()const
		{
			return uint32_t( idxChunk.askedSize / sizeof( uint32_t ) );
		}

		template< typename VertexT >
		uint32_t getVertexCount()const
		{
			return uint32_t( vtxChunk.askedSize / sizeof( VertexT ) );
		}

		uint32_t getBonesCount()const
		{
			return uint32_t( bonChunk.askedSize / sizeof( VertexBoneData ) );
		}

		VkDeviceSize getIndexOffset()const
		{
			return idxChunk.offset;
		}

		VkDeviceSize getVertexOffset()const
		{
			return vtxChunk.offset;
		}

		VkDeviceSize getBonesOffset()const
		{
			return bonChunk.offset;
		}

		uint32_t getFirstIndex()const
		{
			return uint32_t( getIndexOffset() / sizeof( uint32_t ) );
		}

		template< typename VertexT >
		uint32_t getFirstVertex()const
		{
			return uint32_t( getVertexOffset() / sizeof( VertexT ) );
		}

		uint32_t getFirstBone()const
		{
			return uint32_t( getBonesOffset() / sizeof( VertexBoneData ) );
		}
	};
}

#endif
