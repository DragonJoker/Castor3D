/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Buffer/StagingBuffer.hpp"

#include "Image/Texture.hpp"
#include "Image/TextureView.hpp"
#include "Sync/BufferMemoryBarrier.hpp"
#include "Sync/ImageMemoryBarrier.hpp"

namespace renderer
{
	StagingBuffer::StagingBuffer( Device const & device
		, BufferTargets target
		, uint32_t size )
		: m_device{ device }
		, m_buffer{ device.createBuffer( size
			, target | BufferTarget::eTransferSrc
			, MemoryPropertyFlag::eHostVisible ) }
	{
	}

	void StagingBuffer::copyTextureData( CommandBuffer const & commandBuffer
		, ImageSubresourceLayers const & subresourceLayers
		, IVec3 const & offset
		, UIVec3 const & extent
		, ByteArray const & data
		, TextureView const & view )const
	{
		assert( data.size() < getBuffer().getSize() );
		doCopyToStagingBuffer( data.data()
			, uint32_t( data.size() ) );

		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTopOfPipe
				, PipelineStageFlag::eTransfer
				, view.getTexture().makeTransferDestination( view.getSubResourceRange() ) );
			commandBuffer.copyToImage( BufferImageCopy
				{
					0u,
					0u,
					0u,
					subresourceLayers,
					offset,
					UIVec3{
						std::max( 1u, extent[0] ),
						std::max( 1u, extent[1] ),
						std::max( 1u, extent[2] )
					}
				}
				, getBuffer()
				, view );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eFragmentShader
				, view.getTexture().makeShaderInputResource( view.getSubResourceRange() ) );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			res = m_device.getGraphicsQueue().submit( commandBuffer
				, nullptr );

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::copyTextureData( CommandBuffer const & commandBuffer
		, ByteArray const & data
		, TextureView const & view )const
	{
		copyTextureData( commandBuffer
			, {
				getAspectMask( view.getFormat() ),
				view.getSubResourceRange().getBaseMipLevel(),
				view.getSubResourceRange().getBaseArrayLayer(),
				view.getSubResourceRange().getLayerCount()
			}
			, IVec3{ 0, 0, 0 }
			, view.getTexture().getDimensions()
			, data
			, view );
	}

	void StagingBuffer::doCopyToStagingBuffer( uint8_t const * data
		, uint32_t size )const
	{
		assert( size < getBuffer().getSize() );
		auto buffer = static_cast< BufferBase const & >( getBuffer() ).lock( 0
			, size
			, MemoryMapFlag::eWrite | MemoryMapFlag::eInvalidateRange );

		if ( !buffer )
		{
			throw std::runtime_error{ "Staging buffer storage memory mapping failed." };
		}

		std::memcpy( buffer
			, data
			, size );
		static_cast< BufferBase const & >( getBuffer() ).unlock( size, true );
	}

	void StagingBuffer::doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, BufferBase const & buffer )const
	{
		assert( size <= getBuffer().getSize() );
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.copyBuffer( getBuffer()
				, buffer
				, size
				, offset );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			res = m_device.getGraphicsQueue().submit( commandBuffer
				, nullptr );

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, VertexBufferBase const & buffer
		, PipelineStageFlags const & flags )const
	{
		assert( size <= getBuffer().getSize() );
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.copyBuffer( getBuffer()
				, buffer.getBuffer()
				, size
				, offset );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, flags
				, buffer.getBuffer().makeVertexShaderInputResource() );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			res = m_device.getGraphicsQueue().submit( commandBuffer
				, nullptr );

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, UniformBufferBase const & buffer
		, PipelineStageFlags const & flags )const
	{
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.copyBuffer( getBuffer()
				, buffer.getBuffer()
				, size
				, offset );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, flags
				, buffer.getBuffer().makeUniformBufferInput() );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			res = m_device.getGraphicsQueue().submit( commandBuffer
				, nullptr );

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}
}
