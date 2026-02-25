#include "Castor3D/Shader/ShaderAppendBuffer.hpp"

#include "Castor3D/Render/Buffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

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

	void ShaderAppendBuffer::createPassBinding( crg::FramePass & pass
		, uint32_t binding )const
	{
		pass.addInputStorage( *m_buffer->getLastAttach(), binding );
	}

	ashes::WriteDescriptorSet ShaderAppendBuffer::getDescriptorWrite( uint32_t binding )const
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
}
