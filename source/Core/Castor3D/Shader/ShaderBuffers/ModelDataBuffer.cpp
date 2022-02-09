#include "Castor3D/Shader/ShaderBuffers/ModelDataBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslModelData.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementCUSmartPtr( castor3d, ModelDataBuffer )

namespace castor3d
{
	ModelDataBuffer::ModelDataBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_alignedSize{ sizeof( ModelDataUboConfiguration ) }
		, m_buffer{ device.renderSystem
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "ModelDataBuffer" )
			, ashes::QueueShare{}
			, GpuLinearAllocator{ size_t( count ), uint32_t( m_alignedSize ) } }
		, m_stagingBuffer{ std::make_unique< ashes::StagingBuffer >( *device
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, m_alignedSize * count
			, ashes::QueueShare{} ) }
		, m_stagingData{ m_stagingBuffer->getBuffer().lock( 0u
			, m_alignedSize * count
			, 0u ) }
	{
	}

	GpuDataBufferOffset & ModelDataBuffer::getBuffer()
	{
		auto chunk = m_buffer.allocate( m_alignedSize );

		if ( chunk.size == 0 )
		{
			CU_Exception( "GPU buffer failed allocation." );
		}

		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto result = std::make_unique< GpuDataBufferOffset >();
		result->target = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		result->memory = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		result->buffer = this;
		result->chunk = chunk;
		m_allocated.emplace_back( std::move( result ) );
		auto & offset = *m_allocated.back();
		m_dirty.emplace_back( &offset );
		m_connections.emplace( offset.chunk.offset
			, offset.onEdit.connect( [this]( GpuDataBufferOffset const & data )
			{
				m_dirty.emplace_back( &data );
			} ) );
		return offset;
	}

	void ModelDataBuffer::putBuffer( GpuDataBufferOffset const & bufferOffset )
	{
		m_buffer.deallocate( bufferOffset.chunk );
	}

	void ModelDataBuffer::update( ashes::CommandBuffer const & commandBuffer )
	{
		std::vector< GpuDataBufferOffset const * > dirty;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			std::swap( m_dirty, dirty );
		}

		if ( !dirty.empty() )
		{
			std::sort( dirty.begin()
				, dirty.end()
				, []( GpuDataBufferOffset const * lhs, GpuDataBufferOffset const * rhs )
				{
					return lhs->getOffset() < rhs->getOffset();
				} );
			auto & src = m_stagingBuffer->getBuffer();
			auto & dst = m_buffer.getBuffer().getBuffer();
			auto size = dirty.back()->getOffset() + dirty.back()->getSize();
			auto offset = dirty.front()->getOffset();

			m_stagingBuffer->getBuffer().flush( offset
				, size - offset );
			auto dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, dst.makeTransferDestination() );
			commandBuffer.copyBuffer( src
				, dst
				, size - offset
				, offset );
			dstSrcStage = dst.getCompatibleStageFlags();
			commandBuffer.memoryBarrier( dstSrcStage
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				, dst.makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT ) );
		}
	}

	VkDescriptorSetLayoutBinding ModelDataBuffer::createLayoutBinding( uint32_t binding )const
	{
		return makeDescriptorSetLayoutBinding( binding
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) );
	}

	void ModelDataBuffer::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		pass.addInputStorageBuffer( { m_buffer.getBuffer().getBuffer(), "ModelDataBuffer" }
			, binding
			, 0u
			, uint32_t( m_buffer.getBuffer().getBuffer().getSize() ) );
	}

	void ModelDataBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, m_buffer.getBuffer().getBuffer()
			, 0u
			, uint32_t( m_buffer.getBuffer().getBuffer().getSize() ) );
	}
}
