#include "Castor3D/Shader/ShaderAppendBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( castor3d, ShaderAppendBuffer )

namespace castor3d
{
	//*********************************************************************************************

	ShaderAppendBuffer::ShaderAppendBuffer( RenderDevice const & device
		, VkDeviceSize size
		, castor::String const & name )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size + sizeof( uint32_t )
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
		, m_buffer{ makeBufferBase( m_device
			, m_size
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, name ) }
	{
	}

	VkDescriptorSetLayoutBinding ShaderAppendBuffer::createLayoutBinding( uint32_t index
		, VkShaderStageFlags stages )const
	{
		return makeDescriptorSetLayoutBinding( index
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, stages );
	}

	void ShaderAppendBuffer::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		pass.addInputStorageBuffer( { *m_buffer, m_buffer->getName() }
			, binding
			, 0u
			, uint32_t( m_size ) );
	}

	ashes::WriteDescriptorSet ShaderAppendBuffer::getBinding( uint32_t binding )const
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

	void ShaderAppendBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, *m_buffer
			, 0u
			, uint32_t( m_size ) );
	}
}
