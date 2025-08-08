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

namespace c3d
{
	struct ObjectBufferOffset
	{
	public:
		struct GpuBufferChunk
		{
			GpuPackedBaseBuffer * buffer{};
			MemChunk chunk{};

			Buffer const & getBuffer()const
			{
				return buffer->getBuffer();
			}

			bool hasData()const
			{
				return chunk.askedSize != 0;
			}

			uint32_t getAskedSize()const
			{
				return uint32_t( chunk.askedSize );
			}

			uint32_t getAllocSize()const
			{
				return uint32_t( chunk.size );
			}

			template< typename DataT >
			uint32_t getCount()const
			{
				return uint32_t( getAskedSize() / sizeof( DataT ) );
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

			void reset()
			{
				buffer = nullptr;
				chunk = {};
			}

			ashes::WriteDescriptorSet getStorageBinding( uint32_t binding )const
			{
				auto result = ashes::WriteDescriptorSet{ binding
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				result.bufferInfo.push_back( VkDescriptorBufferInfo{ *getBuffer().buffer, getOffset(), getAskedSize() } );
				return result;
			}
		};

		ObjectBufferOffset() = default;

		explicit ObjectBufferOffset( size_t hash )
			: hash{ hash }
		{
		}

		size_t hash{};
		Array< GpuBufferChunk, size_t( SubmeshData::eCount ) > buffers{};
		uint16_t id{};

		explicit operator bool()const
		{
			return hasData( SubmeshData::ePositions );
		}

		GpuBufferChunk & getBufferChunk( SubmeshData data )
		{
			return buffers[uint32_t( data )];
		}

		GpuBufferChunk const & getBufferChunk( SubmeshData data )const
		{
			return buffers[uint32_t( data )];
		}

		uint16_t getID()const
		{
			return id;
		}

		void reset()
		{
			for ( auto & buffer : buffers )
			{
				buffer.reset();
			}
		}

		Buffer const & getBuffer( SubmeshData data )const
		{
			return getBufferChunk( data ).getBuffer();
		}

		bool hasData( SubmeshData data )const
		{
			return getBufferChunk( data ).hasData();
		}

		uint32_t getAskedSize( SubmeshData data )const
		{
			return getBufferChunk( data ).getAskedSize();
		}

		template< typename DataT >
		uint32_t getCount( SubmeshData data )const
		{
			return getBufferChunk( data ).getCount< DataT >();
		}

		VkDeviceSize getOffset( SubmeshData data )const
		{
			return getBufferChunk( data ).getOffset();
		}

		template< typename DataT >
		uint32_t getFirst( SubmeshData data )const
		{
			return getBufferChunk( data ).getFirst< DataT >();
		}

		template< typename IndexT >
		uint32_t getFirstIndex()const
		{
			return getFirst< IndexT >( SubmeshData::eIndex );
		}

		template< typename PositionT >
		uint32_t getFirstVertex()const
		{
			return getFirst< PositionT >( SubmeshData::ePositions );
		}

		void setBufferChunkSize( SubmeshData data, uint32_t size )
		{
			buffers[uint32_t( data )].chunk.askedSize = size;
			buffers[uint32_t( data )].chunk.size = size;
		}

		ashes::WriteDescriptorSet getStorageBinding( SubmeshData data
			, uint32_t binding )const
		{
			return getBufferChunk( data ).getStorageBinding( binding );
		}
	};
}

#endif
