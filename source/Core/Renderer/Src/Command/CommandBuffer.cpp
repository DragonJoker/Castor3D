/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Command/CommandBuffer.hpp"

#include "Buffer/VertexBuffer.hpp"
#include "Buffer/UniformBuffer.hpp"
#include "Image/ImageSubresourceRange.hpp"
#include "Image/Texture.hpp"
#include "Image/TextureView.hpp"

namespace renderer
{
	CommandBuffer::CommandBuffer( Device const & device
		, CommandPool const & pool
		, bool primary )
	{
	}

	void CommandBuffer::copyBuffer( BufferBase const & src
		, BufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src, dst );
	}

	void CommandBuffer::copyBuffer( BufferBase const & src
		, VertexBufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src, dst.getBuffer() );
	}

	void CommandBuffer::copyBuffer( VertexBufferBase const & src
		, BufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src.getBuffer(), dst );
	}

	void CommandBuffer::copyBuffer( VertexBufferBase const & src
		, VertexBufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src.getBuffer(), dst.getBuffer() );
	}

	void CommandBuffer::copyBuffer( BufferBase const & src
		, UniformBufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src, dst.getBuffer() );
	}

	void CommandBuffer::copyBuffer( UniformBufferBase const & src
		, BufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src.getBuffer(), dst );
	}

	void CommandBuffer::copyBuffer( UniformBufferBase const & src
		, UniformBufferBase const & dst
		, uint32_t size
		, uint32_t offset )const
	{
		BufferCopy copyInfo
		{
			offset,
			0,
			size
		};
		copyBuffer( copyInfo, src.getBuffer(), dst.getBuffer() );
	}

	void CommandBuffer::copyImage( TextureView const & src
		, TextureView const & dst )const
	{
		auto const & srcRange = src.getSubResourceRange();
		auto const & dstRange = dst.getSubResourceRange();
		copyImage( ImageCopy
			{
				{                                                   // srcSubresource
					getAspectMask( src.getFormat() ),
					srcRange.getBaseMipLevel(),
					srcRange.getBaseArrayLayer(),
					srcRange.getLayerCount()
				},
				IVec3{                                              // srcOffset
					0,                                                  // x
					0,                                                  // y
					0                                                   // z
				},
				{                                                   // dstSubresource
					getAspectMask( dst.getFormat() ),
					dstRange.getBaseMipLevel(),
					dstRange.getBaseArrayLayer(),
					dstRange.getLayerCount()
				},
				IVec3{                                              // dstOffset
					0,                                                  // x
					0,                                                  // y
					0                                                   // z
				},
				UIVec3{                                             // extent
					uint32_t( dst.getTexture().getDimensions()[0] ),
					uint32_t( dst.getTexture().getDimensions()[1] ),
					uint32_t( dst.getTexture().getDimensions()[2] ),
				}
			}
			, src
			, dst );
	}
}
