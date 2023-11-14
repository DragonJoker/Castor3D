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
			GpuPackedBaseBuffer * buffer{};
			MemChunk chunk{};

			ashes::BufferBase const & getBuffer()const
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
				chunk.offset = 0u;
				chunk.askedSize = 0u;
				chunk.size = 0u;
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
					, getAskedSize() );
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
					, getAskedSize() );
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
					, getAskedSize() );
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
					, getAskedSize() );
			}

			ashes::WriteDescriptorSet getUniformBinding( uint32_t binding )const
			{
				auto result = ashes::WriteDescriptorSet{ binding
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
				result.bufferInfo.push_back( { getBuffer(), getOffset(), getAskedSize() } );
				return result;
			}

			ashes::WriteDescriptorSet getStorageBinding( uint32_t binding )const
			{
				auto result = ashes::WriteDescriptorSet{ binding
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
				result.bufferInfo.push_back( { getBuffer(), getOffset(), getAskedSize() } );
				return result;
			}
		};

		size_t hash{};
		std::array< GpuBufferChunk, size_t( SubmeshData::eCount ) > buffers{};

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

		void reset()
		{
			for ( auto & buffer : buffers )
			{
				buffer.reset();
			}
		}

		ashes::BufferBase const & getBuffer( SubmeshData data )const
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

		void createUniformPassBinding( SubmeshData data
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( data ).createUniformPassBinding( pass
				, binding
				, name );
		}

		void createInputStoragePassBinding( SubmeshData data
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( data ).createInputStoragePassBinding( pass
				, binding
				, name );
		}

		void createInOutStoragePassBinding( SubmeshData data
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( data ).createInOutStoragePassBinding( pass
				, binding
				, name );
		}

		void createOutputStoragePassBinding( SubmeshData data
			, crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			getBufferChunk( data ).createOutputStoragePassBinding( pass
				, binding
				, name );
		}

		ashes::WriteDescriptorSet getUniformBinding( SubmeshData data
			, uint32_t binding )const
		{
			return getBufferChunk( data ).getUniformBinding( binding );
		}

		ashes::WriteDescriptorSet getStorageBinding( SubmeshData data
			, uint32_t binding )const
		{
			return getBufferChunk( data ).getStorageBinding( binding );
		}
	};
}

#endif
