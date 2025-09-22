/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferOffset_HPP___
#define ___C3D_GpuBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UploadData.hpp"

#include "Castor3D/Render/Buffer.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace c3d
{
	template< typename DataT >
	struct GpuBufferOffsetT
	{
	public:
		GpuBufferBase * pool{};
		Buffer buffer{};
		BufferUsageFlags target{};
		MemoryPropertyFlags memory{};
		MemChunk chunk{};

		GpuBufferOffsetT()noexcept = default;
		GpuBufferOffsetT( GpuBufferBase & pool
			, Buffer buffer
			, BufferUsageFlags target
			, MemoryPropertyFlags memory
			, MemChunk chunk )noexcept
			: pool{ &pool }
			, buffer{ std::move( buffer ) }
			, target{ target }
			, memory{ memory }
			, chunk{ std::move( chunk ) }
		{
		}

		void setPool( GpuBufferBase & ppool )
		{
			pool = &ppool;
		}

		explicit operator bool()const
		{
			return pool
				&& pool->hasBuffer();
		}

		GpuBufferBase const & getPool()const
		{
			return *pool;
		}

		GpuBufferBase & getPool()
		{
			return *pool;
		}

		Buffer const & getBuffer()const
		{
			return buffer;
		}

		Buffer & getBuffer()
		{
			return buffer;
		}

		DeviceSize getCount()const
		{
			return chunk.askedSize / sizeof( DataT );
		}

		DeviceSize getSize()const
		{
			return chunk.size;
		}

		DeviceSize getOffset()const
		{
			return chunk.offset;
		}

		ArrayView< DataT > getData()
		{
			using DataPtr = DataT *;
			return makeArrayView( reinterpret_cast< DataPtr >( pool->getDatas().data() + getOffset() )
				, getCount() );
		}

		void upload( UploadData & uploader
			, AccessState dstAccessState )const
		{
			pool->upload( uploader
				, getOffset(), getSize()
				, c3d::move( dstAccessState ) );
		}

		void markDirty( DeviceSize size
			, AccessState dstAccessState )const
		{
			pool->markDirty( getOffset()
				, std::min( size, getSize() )
				, c3d::move( dstAccessState ) );
		}

		void markDirty( DeviceSize size
			, AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )const
		{
			pool->markDirty( getOffset()
				, std::min( size, getSize() )
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void markDirty( AccessState dstAccessState )const
		{
			markDirty( getSize(), c3d::move( dstAccessState ) );
		}

		void markDirty( AccessFlags dstAccessFlags
			, PipelineStageFlags dstPipelineFlags )const
		{
			markDirty( getSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}

		VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index
			, VkShaderStageFlags stages )const
		{
			return makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages );
		}

		ashes::WriteDescriptorSet getStorageBinding( uint32_t binding )const
		{
			auto result = ashes::WriteDescriptorSet{ binding
				, 0u
				, 1u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
			result.bufferInfo.push_back( { *getBuffer().buffer, getOffset(), getSize() } );
			return result;
		}

		void createBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & binding )const
		{
			descriptorSet.createBinding( binding
				, *getBuffer().buffer
				, uint32_t( getOffset() )
				, uint32_t( getSize() ) );
		}
	};
}

#endif
