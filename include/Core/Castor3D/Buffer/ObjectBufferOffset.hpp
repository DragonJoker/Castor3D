/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectBufferOffset_HPP___
#define ___C3D_ObjectBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferOffset.hpp"
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

			template< typename DataT >
			uint32_t getFirst()const
			{
				return uint32_t( getOffset() / sizeof( DataT ) );
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

			void createUniformPassBinding( crg::FramePass & pass
				, uint32_t binding
				, std::string const & name )const
			{
				castor3d::createUniformPassBinding( pass
					, binding
					, name
					, getBuffer()
					, getOffset()
					, getSize() );
			}

			void createInputStoragePassBinding( crg::FramePass & pass
				, uint32_t binding
				, std::string const & name )const
			{
				castor3d::createInputStoragePassBinding( pass
					, binding
					, name
					, getBuffer()
					, getOffset()
					, getSize() );
			}

			void createInOutStoragePassBinding( crg::FramePass & pass
				, uint32_t binding
				, std::string const & name )const
			{
				castor3d::createInOutStoragePassBinding( pass
					, binding
					, name
					, getBuffer()
					, getOffset()
					, getSize() );
			}

			void createOutputStoragePassBinding( crg::FramePass & pass
				, uint32_t binding
				, std::string const & name )const
			{
				castor3d::createOutputStoragePassBinding( pass
					, binding
					, name
					, getBuffer()
					, getOffset()
					, getSize() );
			}

			ashes::WriteDescriptorSet getUniformBinding( uint32_t binding )const
			{
				auto result = ashes::WriteDescriptorSet{ binding
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
				result.bufferInfo.push_back( { getBuffer(), getOffset(), getSize() } );
				return result;
			}

			ashes::WriteDescriptorSet getStorageBinding( uint32_t binding )const
			{
				auto result = ashes::WriteDescriptorSet{ binding
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				result.bufferInfo.push_back( { getBuffer(), getOffset(), getSize() } );
				return result;
			}
		};

		size_t hash{};
		std::array< GpuBufferChunk, size_t( SubmeshData::eCount ) > buffers{};

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

		template< typename DataT >
		uint32_t getFirst( SubmeshFlag flag )const
		{
			return getBufferChunk( flag ).getFirst< DataT >();
		}

		template< typename IndexT >
		uint32_t getFirstIndex()const
		{
			return getFirst< IndexT >( SubmeshFlag::eIndex );
		}

		template< typename PositionT >
		uint32_t getFirstVertex()const
		{
			return getFirst< PositionT >( SubmeshFlag::ePositions );
		}

		void markDirty( SubmeshFlag flag
			, VkAccessFlags dstAccessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VkPipelineStageFlags dstPipelineFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT )const
		{
			getBufferChunk( flag ).markDirty( dstAccessFlags
				, dstPipelineFlags );
		}

		void createUniformPassBinding( SubmeshFlag flag
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( flag ).createUniformPassBinding( pass
				, binding
				, name );
		}

		void createInputStoragePassBinding( SubmeshFlag flag
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( flag ).createInputStoragePassBinding( pass
				, binding
				, name );
		}

		void createInOutStoragePassBinding( SubmeshFlag flag
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( flag ).createInOutStoragePassBinding( pass
				, binding
				, name );
		}

		void createOutputStoragePassBinding( SubmeshFlag flag
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( flag ).createOutputStoragePassBinding( pass
				, binding
				, name );
		}

		ashes::WriteDescriptorSet getUniformBinding( SubmeshFlag flag
			, uint32_t binding )const
		{
			return getBufferChunk( flag ).getUniformBinding( binding );
		}

		ashes::WriteDescriptorSet getStorageBinding( SubmeshFlag flag
			, uint32_t binding )const
		{
			return getBufferChunk( flag ).getStorageBinding( binding );
		}
	};
}

#endif
