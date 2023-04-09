#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( castor3d, ShaderBuffer )

namespace castor3d
{
	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, RenderDevice const & device
		, VkDeviceSize size
		, castor::String name
		, crg::AccessState wantedState )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size + 4 * sizeof( uint32_t )
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
		, m_wantedState{ std::move( wantedState ) }
		, m_buffer{ makeBufferBase( m_device
			, m_size
			, ( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT )
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name ) }
		, m_staging{ std::make_unique< ashes::StagingBuffer >( *m_device
			, name + "Staging"
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, m_size
			, ashes::QueueShare{ { m_device.getGraphicsQueueFamilyIndex()
				, m_device.getComputeQueueFamilyIndex()
				, m_device.getTransferQueueFamilyIndex() } } ) }
		, m_rawData( m_staging->getBuffer().lock( 0u, m_size, 0u ) )
		, m_data{ ( m_rawData + 4u * sizeof( uint32_t ) ) }
		, m_counts{ castor::makeArrayView( reinterpret_cast< uint32_t * >( m_rawData )
			, reinterpret_cast< uint32_t * >( m_data ) ) }
	{
		CU_Require( m_rawData );
	}

	void ShaderBuffer::upload( ashes::CommandBuffer const & commandBuffer )const
	{
		doUpload( commandBuffer
			, 0u
			, ashes::WholeSize );
	}

	void ShaderBuffer::upload( ashes::CommandBuffer const & commandBuffer
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		doUpload( commandBuffer
			, 0u
			, std::min( m_size
				, ashes::getAlignedSize( size
					, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) ) );
	}

	VkDescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index
		, VkShaderStageFlags stages )const
	{
		return makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stages );
	}

	void ShaderBuffer::createPassBinding( crg::FramePass & pass
		, castor::String const & name
		, uint32_t binding )const
	{
		pass.addInputStorageBuffer( { *m_buffer, name }
			, binding
			, 0u
			, uint32_t( m_size ) );
	}

	ashes::WriteDescriptorSet ShaderBuffer::getSingleBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer
			, offset + sizeof( uint32_t ) * 4u
			, size } );
		return result;
	}

	ashes::WriteDescriptorSet ShaderBuffer::getBinding( uint32_t binding )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer
			, 0u
			, m_size } );
		return result;
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, *m_buffer
			, 0u
			, uint32_t( m_size ) );
	}

	void ShaderBuffer::doUpload( ashes::CommandBuffer const & commandBuffer
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		CU_Require( ( offset + size <= m_size )
			|| ( offset == 0u && size == ashes::WholeSize ) );
		size = ( size == ashes::WholeSize ? m_size : size );
		m_staging->getBuffer().flush( offset, size );
		auto stgSrcStage = m_staging->getBuffer().getCompatibleStageFlags();
		commandBuffer.memoryBarrier( stgSrcStage
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_staging->getBuffer().makeTransferSource() );

		auto dstSrcStage = m_buffer->getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstSrcStage
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_buffer->makeTransferDestination() );

		commandBuffer.copyBuffer( m_staging->getBuffer()
			, *m_buffer
			, size
			, offset
			, 0u );

		auto dstDstStage = m_buffer->getCompatibleStageFlags();
		commandBuffer.memoryBarrier( dstDstStage
			, m_wantedState.pipelineStage
			, m_buffer->makeMemoryTransitionBarrier( m_wantedState.access ) );

		auto stgDstStage = m_staging->getBuffer().getCompatibleStageFlags();
		commandBuffer.memoryBarrier( stgDstStage
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_staging->getBuffer().makeHostWrite() );
	}
}
