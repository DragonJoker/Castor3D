/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferOffset_HPP___
#define ___C3D_ObjectBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPackedAllocator.hpp"
#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

#include <unordered_map>

namespace castor3d
{
	struct ObjectBufferOffset
	{
	public:
		struct GpuBufferChunk
		{
			GpuPackedBuffer * buffer{};
			MemChunk chunk{};

			ashes::BufferBase const & getBuffer()const
			{
				return buffer->getBuffer().getBuffer();
			}

			bool hasData()const
			{
				return chunk.askedSize != 0;
			}

			uint32_t getSize()const
			{
				return uint32_t( chunk.askedSize );
			}

			template< typename DataT >
			castor::ArrayView< DataT > getData()const
			{
				CU_Require( hasData() );
				return castor::makeArrayView( reinterpret_cast< DataT * >( buffer->getDatas().data() + getOffset() )
					, uint32_t( chunk.askedSize / sizeof( DataT ) ) );
			}

			template< typename DataT >
			uint32_t getCount()const
			{
				return uint32_t( getSize() / sizeof( DataT ) );
			}

			VkDeviceSize getOffset()const
			{
				return chunk.offset;
			}

			void markDirty( VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
			{
				buffer->markDirty( getOffset()
					, getSize()
					, dstAccessFlags
					, dstPipelineFlags );
			}

			void reset()
			{
				buffer = nullptr;
				chunk.offset = 0u;
				chunk.askedSize = 0u;
				chunk.size = 0u;
			}

			void directUpload( ashes::Queue const & queue
				, ashes::CommandPool const & commandPool
				, VkAccessFlags dstAccessFlags
				, VkPipelineStageFlags dstPipelineFlags )
			{
				buffer->uploadDirect( queue
					, commandPool
					, chunk.offset
					, chunk.askedSize
					, dstAccessFlags
					, dstPipelineFlags);
			}
		};

		SubmeshFlags submeshFlags{};
		std::array< GpuBufferChunk, getIndex( SubmeshFlag::eAllComponents ) > buffers{};

		explicit operator bool()const
		{
			return hasData( SubmeshFlag::ePositions );
		}

		GpuBufferChunk & getBufferChunk( SubmeshFlag flag )
		{
			return buffers[getIndex( flag )];
		}

		GpuBufferChunk const & getBufferChunk( SubmeshFlag flag )const
		{
			return buffers[getIndex( flag )];
		}

		void reset()
		{
			for ( auto & buffer : buffers )
			{
				buffer.reset();
			}
		}

		void directUpload( SubmeshFlag flag
			, ashes::Queue const & queue
			, ashes::CommandPool const & commandPool
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags )
		{
			getBufferChunk( flag ).directUpload( queue
				, commandPool
				, dstAccessFlags
				, dstPipelineFlags );
		}

		ashes::BufferBase const & getBuffer( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getBuffer();
		}

		bool hasData( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).hasData();
		}

		template< typename DataT >
		castor::ArrayView< DataT > getData( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getData< DataT >();
		}

		uint32_t getSize( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getSize();
		}

		template< typename DataT >
		uint32_t getCount( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getCount< DataT >();
		}

		VkDeviceSize getOffset( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getOffset();
		}

		template< typename IndexT >
		uint32_t getFirstIndex()const
		{
			return uint32_t( getOffset( SubmeshFlag::eIndex ) / sizeof( IndexT ) );
		}

		template< typename PositionT >
		uint32_t getFirstVertex()const
		{
			return uint32_t( getOffset( SubmeshFlag::ePositions ) / sizeof( PositionT ) );
		}

		void markDirty( SubmeshFlag flag
			, VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			getBufferChunk( flag ).markDirty( dstAccessFlags
				, dstPipelineFlags );
		}
	};
}

#endif
