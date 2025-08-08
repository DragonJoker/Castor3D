#include "Castor3D/Shader/ShaderAppendBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <RenderGraph/FramePass.hpp>

CU_ImplementSmartPtr( c3d, ShaderAppendBuffer )

namespace c3d
{
	//*********************************************************************************************

	ShaderAppendBuffer::ShaderAppendBuffer( RenderDevice const & device
		, crg::ResourcesCache & resources
		, VkDeviceSize size
		, String const & name )
		: m_device{ device }
		, m_size{ ashes::getAlignedSize( size + sizeof( uint32_t )
			, m_device.renderSystem.getValue( GpuMin::eBufferMapSize ) ) }
		, m_buffer{ makeBufferBase( m_device, resources
			, m_size
			, BufferUsageFlags::eStorageBuffer
			, MemoryPropertyFlags::eDeviceLocal
			, name ) }
	{
		m_buffer->create();
	}

	ShaderAppendBuffer::~ShaderAppendBuffer()noexcept
	{
		m_buffer->destroy();
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
		pass.addInputStorage( *m_buffer->getLastAttach(), binding );
	}

	ashes::WriteDescriptorSet ShaderAppendBuffer::getBinding( uint32_t binding )const
	{
		auto result = ashes::WriteDescriptorSet{ binding
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.push_back( VkDescriptorBufferInfo{ m_buffer->getBuffer()
			, 0u
			, m_size } );
		return result;
	}

	void ShaderAppendBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		descriptorSet.createBinding( binding
			, *m_buffer->buffer
			, 0u, uint32_t( m_size ) );
	}
}
