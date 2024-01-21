/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferOffset_HPP___
#define ___C3D_GpuBufferOffset_HPP___

#include "Castor3D/Buffer/GpuBuffer.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

namespace castor3d
{
	C3D_API void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > const & buffers
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > const & buffers
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > const & buffers
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > const & buffers
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > const & buffers
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );
	C3D_API void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size );

	template< typename DataT >
	struct GpuBufferOffsetT
	{
	public:
		GpuBufferBase * buffer{};
		VkBufferUsageFlags target{};
		VkMemoryPropertyFlags memory{};
		MemChunk chunk{};

		GpuBufferOffsetT()noexcept = default;

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
			using DataPtr = DataT *;
			return castor::makeArrayView( DataPtr( buffer->getDatas().data() + getOffset() )
				, getCount() );
		}

		void markDirty( VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags )const
		{
			markDirty( getSize()
				, dstAccessFlags
				, dstPipelineFlags );
		}

		void markDirty( VkDeviceSize size
			, VkAccessFlags dstAccessFlags
			, VkPipelineStageFlags dstPipelineFlags )const
		{
			buffer->markDirty( getOffset()
				, std::min( size, getSize() )
				, dstAccessFlags
				, dstPipelineFlags );
		}

		VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index
			, VkShaderStageFlags stages )const
		{
			return castor3d::makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, stages );
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

		void createClearableOutputStorageBinding( crg::FramePass & pass
			, uint32_t binding
			, std::string const & name )const
		{
			castor3d::createClearableOutputStorageBinding( pass
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

		void createBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & binding )const
		{
			descriptorSet.createBinding( binding
				, getBuffer()
				, uint32_t( getOffset() )
				, uint32_t( getSize() ) );
		}
	};
}

#endif
