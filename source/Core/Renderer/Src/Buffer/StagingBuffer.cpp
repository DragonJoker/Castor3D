/*
This file belongs to RendererLib.
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

	void StagingBuffer::uploadTextureData( CommandBuffer const & commandBuffer
		, ImageSubresourceLayers const & subresourceLayers
		, Offset3D const & offset
		, Extent3D const & extent
		, uint8_t const * const data
		, uint32_t size
		, TextureView const & view )const
	{
		assert( size <= getBuffer().getSize() );
		doCopyToStagingBuffer( data, size );

		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTopOfPipe
				, PipelineStageFlag::eTransfer
				, view.makeTransferDestination( ImageLayout::eUndefined
					, 0u ) );
			commandBuffer.copyToImage( BufferImageCopy
				{
					0u,
					0u,
					0u,
					subresourceLayers,
					offset,
					Extent3D{
						std::max( 1u, extent.width ),
						std::max( 1u, extent.height ),
						std::max( 1u, extent.depth )
					}
				}
				, getBuffer()
				, view.getTexture() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eFragmentShader
				, view.makeShaderInputResource( ImageLayout::eTransferDstOptimal
					, renderer::AccessFlag::eTransferWrite ) );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::uploadTextureData( CommandBuffer const & commandBuffer
		, uint8_t const * const data
		, uint32_t size
		, TextureView const & view )const
	{
		uploadTextureData( commandBuffer
			, {
				getAspectMask( view.getFormat() ),
				view.getSubResourceRange().baseMipLevel,
				view.getSubResourceRange().baseArrayLayer,
				view.getSubResourceRange().layerCount
			}
			, Offset3D{ 0, 0, 0 }
			, view.getTexture().getDimensions()
			, data
			, size
			, view );
	}

	void StagingBuffer::downloadTextureData( CommandBuffer const & commandBuffer
		, ImageSubresourceLayers const & subresourceLayers
		, Offset3D const & offset
		, Extent3D const & extent
		, uint8_t * data
		, uint32_t size
		, TextureView const & view )const
	{
		assert( size <= getBuffer().getSize() );

		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTopOfPipe
				, PipelineStageFlag::eTransfer
				, view.makeTransferSource( ImageLayout::eUndefined
					, 0u ) );
			commandBuffer.copyToBuffer( BufferImageCopy
				{
					0u,
					0u,
					0u,
					subresourceLayers,
					offset,
					Extent3D{
						std::max( 1u, extent.width ),
						std::max( 1u, extent.height ),
						std::max( 1u, extent.depth )
					}
				}
				, view.getTexture()
				, getBuffer() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eFragmentShader
				, view.makeShaderInputResource( ImageLayout::eTransferSrcOptimal
					, renderer::AccessFlag::eTransferRead ) );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
			doCopyFromStagingBuffer( data, size );
		}
	}

	void StagingBuffer::downloadTextureData( CommandBuffer const & commandBuffer
		, uint8_t * data
		, uint32_t size
		, TextureView const & view )const
	{
		downloadTextureData( commandBuffer
			, {
				getAspectMask( view.getFormat() ),
				view.getSubResourceRange().baseMipLevel,
				view.getSubResourceRange().baseArrayLayer,
				view.getSubResourceRange().layerCount
			}
			, Offset3D{ 0, 0, 0 }
			, view.getTexture().getDimensions()
			, data
			, size
			, view );
	}

	void StagingBuffer::doCopyToStagingBuffer( uint8_t const * data
		, uint32_t size )const
	{
		assert( size <= getBuffer().getSize() );
		auto buffer = static_cast< BufferBase const & >( getBuffer() ).lock( 0u
			, size
			, MemoryMapFlag::eWrite | MemoryMapFlag::eInvalidateRange );

		if ( !buffer )
		{
			throw std::runtime_error{ "Staging buffer storage memory mapping failed." };
		}

		std::memcpy( buffer
			, data
			, size );
		getBuffer().flush( 0u, size );
		getBuffer().unlock();
	}

	void StagingBuffer::doCopyFromStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, BufferBase const & buffer )const
	{
		assert( size <= getBuffer().getSize() );
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.makeTransferDestination() );
			commandBuffer.copyBuffer( getBuffer()
				, buffer
				, size
				, offset );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

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
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.getBuffer().makeTransferDestination() );
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

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

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
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.getBuffer().makeTransferDestination() );
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

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::doCopyFromStagingBuffer( uint8_t * data
		, uint32_t size )const
	{
		assert( size <= getBuffer().getSize() );
		auto buffer = static_cast< BufferBase const & >( getBuffer() ).lock( 0u
			, size
			, MemoryMapFlag::eRead );

		if ( !buffer )
		{
			throw std::runtime_error{ "Staging buffer storage memory mapping failed." };
		}

		std::memcpy( data
			, buffer
			, size );
		getBuffer().flush( 0u, size );
		getBuffer().unlock();
	}

	void StagingBuffer::doCopyToStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, BufferBase const & buffer )const
	{
		assert( size <= getBuffer().getSize() );
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferDestination() );
			commandBuffer.copyBuffer( buffer
				, getBuffer()
				, size
				, offset );
			bool res = commandBuffer.end();

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Texture data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::doCopyToStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, VertexBufferBase const & buffer
		, PipelineStageFlags const & flags )const
	{
		assert( size <= getBuffer().getSize() );
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.getBuffer().makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferDestination() );
			commandBuffer.copyBuffer( buffer.getBuffer()
				, getBuffer()
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

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}

	void StagingBuffer::doCopyToStagingBuffer( CommandBuffer const & commandBuffer
		, uint32_t size
		, uint32_t offset
		, UniformBufferBase const & buffer
		, PipelineStageFlags const & flags )const
	{
		if ( commandBuffer.begin( CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, buffer.getBuffer().makeTransferSource() );
			commandBuffer.memoryBarrier( PipelineStageFlag::eTransfer
				, PipelineStageFlag::eTransfer
				, getBuffer().makeTransferDestination() );
			commandBuffer.copyBuffer( buffer.getBuffer()
				, getBuffer()
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

			auto fence = m_device.createFence();
			res = m_device.getGraphicsQueue().submit( commandBuffer
				, fence.get() );
			fence->wait( FenceTimeout );

			if ( !res )
			{
				throw std::runtime_error{ "Buffer data copy failed." };
			}

			m_device.getGraphicsQueue().waitIdle();
		}
	}
}
