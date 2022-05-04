/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferOffset_HPP___
#define ___C3D_ObjectBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"
#include "Castor3D/Model/VertexGroup.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

namespace castor3d
{
	struct ObjectBufferOffset
	{
	public:
		GpuPackedBuffer * vtxBuffer{};
		GpuPackedBuffer * idxBuffer{};
		GpuPackedBuffer * bonBuffer{};
		GpuPackedBuffer * mphBuffer{};
		MemChunk vtxChunk{};
		MemChunk idxChunk{};
		MemChunk bonChunk{};
		MemChunk mphChunk{};

		explicit operator bool()const
		{
			return vtxBuffer != nullptr;
		}

		ashes::BufferBase const & getIndexBuffer()const
		{
			return idxBuffer->getBuffer().getBuffer();
		}

		ashes::BufferBase const & getVertexBuffer()const
		{
			return vtxBuffer->getBuffer().getBuffer();
		}

		ashes::BufferBase const & getBonesBuffer()const
		{
			return bonBuffer->getBuffer().getBuffer();
		}

		ashes::BufferBase const & getMorphBuffer()const
		{
			return mphBuffer->getBuffer().getBuffer();
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

		bool hasMorph()const
		{
			return mphChunk.askedSize != 0;
		}

		template< typename IndexT >
		castor::ArrayView< IndexT > getIndexData()const
		{
			CU_Require( hasIndices() );
			return castor::makeArrayView( reinterpret_cast< IndexT * >( idxBuffer->getDatas().data() + getIndexOffset() )
				, uint32_t( idxChunk.askedSize / sizeof( IndexT ) ) );
		}

		template< typename VertexT >
		castor::ArrayView< VertexT > getVertexData()const
		{
			CU_Require( hasVertices() );
			return castor::makeArrayView( reinterpret_cast< VertexT * >( vtxBuffer->getDatas().data() + getVertexOffset() )
				, getVertexCount< VertexT >() );
		}

		castor::ArrayView< VertexBoneData > getBoneData()const
		{
			CU_Require( hasBones() );
			return castor::makeArrayView( reinterpret_cast< VertexBoneData * >( bonBuffer->getDatas().data() + getBonesOffset() )
				, getBonesCount() );
		}

		template< typename VertexT >
		castor::ArrayView< VertexT > getMorphData()const
		{
			CU_Require( hasMorph() );
			return castor::makeArrayView( reinterpret_cast< VertexT * >( mphBuffer->getDatas().data() + getMorphOffset() )
				, getMorphCount< VertexT >() );
		}

		uint32_t getIndexSize()const
		{
			return uint32_t( idxChunk.askedSize );
		}

		uint32_t getVertexSize()const
		{
			return uint32_t( vtxChunk.askedSize );
		}

		uint32_t getBonesSize()const
		{
			return uint32_t( bonChunk.askedSize );
		}

		uint32_t getMorphSize()const
		{
			return uint32_t( mphChunk.askedSize );
		}

		uint32_t getIndexCount()const
		{
			return uint32_t( getIndexSize() / sizeof( uint32_t ) );
		}

		template< typename VertexT >
		uint32_t getVertexCount()const
		{
			return uint32_t( getVertexSize() / sizeof( VertexT ) );
		}

		uint32_t getBonesCount()const
		{
			return uint32_t( getBonesSize() / sizeof( VertexBoneData ) );
		}

		template< typename VertexT >
		uint32_t getMorphCount()const
		{
			return uint32_t( getMorphSize() / sizeof( VertexT ) );
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

		VkDeviceSize getMorphOffset()const
		{
			return mphChunk.offset;
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

		template< typename VertexT >
		uint32_t getFirstMorph()const
		{
			return uint32_t( getMorphOffset() / sizeof( VertexT ) );
		}

		void markVertexDirty( VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			vtxBuffer->markDirty( getVertexOffset()
				, getVertexSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void markIndexDirty( VkAccessFlags dstAccessFlags = VK_ACCESS_INDEX_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			idxBuffer->markDirty( getIndexOffset()
				, getIndexSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void markBonesDirty( VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			bonBuffer->markDirty( getBonesOffset()
				, getBonesSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void markMorphDirty( VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			mphBuffer->markDirty( getMorphOffset()
				, getMorphSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}
	};
}

#endif
