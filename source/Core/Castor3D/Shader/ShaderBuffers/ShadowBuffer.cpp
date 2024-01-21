#include "Castor3D/Shader/ShaderBuffers/ShadowBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

CU_ImplementSmartPtr( castor3d, ShadowBuffer )

namespace castor3d
{
	ShadowBuffer::ShadowBuffer( RenderDevice const & device )
		: m_device{ device }
		, m_buffer{ m_device.uboPool->getBuffer< AllShadowData >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ) }
	{
	}

	ShadowBuffer::~ShadowBuffer()noexcept
	{
		m_device.uboPool->putBuffer( m_buffer );
	}

	VkDescriptorSetLayoutBinding ShadowBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return makeDescriptorSetLayoutBinding( binding
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, stages );
	}

	void ShadowBuffer::createPassBinding( crg::FramePass & pass, uint32_t binding )const
	{
		return m_buffer.createPassBinding( pass, cuT( "C3D_Shadows" ), binding );
	}

	ashes::WriteDescriptorSet ShadowBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getDescriptorWrite( binding, 0u );
	}

	void ShadowBuffer::addBinding( ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & binding )const
	{
		descriptorWrites.emplace_back( getBinding( binding ) );
		++binding;
	}

	void ShadowBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createSizedBinding( descriptorSet, binding );
	}
}
