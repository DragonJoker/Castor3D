#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementCUSmartPtr( castor3d, ShaderBuffer )

namespace castor3d
{
	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t size
		, castor::String name )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
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
		, m_data( m_staging->getBuffer().lock( 0u, m_size, 0u ) )
	{
		assert( m_data );
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

	VkDescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		return makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) );
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

	ashes::WriteDescriptorSet ShaderBuffer::getBinding( uint32_t binding
		, VkDeviceSize offset
		, VkDeviceSize size )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer, offset, size } );
		return result;
	}

	ashes::WriteDescriptorSet ShaderBuffer::getBinding( uint32_t binding )const
	{
		return getBinding( binding, 0u, m_size );
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
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_buffer->makeMemoryTransitionBarrier( VK_ACCESS_SHADER_READ_BIT ) );

		auto stgDstStage = m_staging->getBuffer().getCompatibleStageFlags();
		commandBuffer.memoryBarrier( stgDstStage
			, VK_PIPELINE_STAGE_HOST_BIT
			, m_staging->getBuffer().makeHostWrite() );
	}
}
