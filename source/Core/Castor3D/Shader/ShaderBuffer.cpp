#include "Castor3D/Shader/ShaderBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::BufferBasePtr doCreateBuffer( Engine & engine
			, RenderDevice const & device
			, VkDeviceSize size
			, castor::String name )
		{
			ashes::BufferBasePtr result;
			auto & renderSystem = *engine.getRenderSystem();
			VkBufferUsageFlagBits target = renderSystem.getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
					? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					: VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
			result = makeBufferBase( device
				, size
				, target | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, std::move( name ) );
			return result;
		}

		ashes::BufferViewPtr doCreateView( Engine & engine
			, RenderDevice const & device
			, VkFormat tboFormat
			, ashes::BufferBase const & buffer )
		{
			ashes::BufferViewPtr result;

			if ( !engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = device->createBufferView( buffer
					, tboFormat
					, 0u
					, uint32_t( buffer.getSize() ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t size
		, castor::String name
		, VkFormat tboFormat )
		: m_device{ device }
		, m_size{ size }
		, m_buffer{ doCreateBuffer( engine, device, m_size, name ) }
		, m_bufferView{ doCreateView( engine, device, tboFormat, *m_buffer ) }
		, m_type{ m_bufferView ? VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }
		, m_data( size_t( m_size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_bufferView.reset();
		m_buffer.reset();
	}

	void ShaderBuffer::update()
	{
		doUpdate( 0u, ashes::WholeSize );
	}

	void ShaderBuffer::update( VkDeviceSize offset
		, VkDeviceSize size )
	{
		doUpdate( 0u
			, std::min( m_size
				, ashes::getAlignedSize( size
					, uint32_t( m_device.properties.limits.nonCoherentAtomSize ) ) ) );
	}

	VkDescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		return makeDescriptorSetLayoutBinding( index, m_type, VK_SHADER_STAGE_FRAGMENT_BIT );
	}

	ashes::WriteDescriptorSet ShaderBuffer::getBinding( uint32_t binding )const
	{
		auto result = ashes::WriteDescriptorSet
		{
			binding,
			0u,
			1u,
			( checkFlag( m_buffer->getUsage(), VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
				? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ),
		};

		if ( m_bufferView )
		{
			result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer, m_bufferView->getOffset(), m_bufferView->getRange() } );
			result.texelBufferView.push_back( *m_bufferView );
		}
		else
		{
			result.bufferInfo.push_back( VkDescriptorBufferInfo{ *m_buffer, 0u, uint32_t( m_size ) } );
		}

		return result;
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		if ( m_bufferView )
		{
			descriptorSet.createBinding( binding
				, *m_buffer
				, *m_bufferView
				, 0u );
		}
		else
		{
			descriptorSet.createBinding( binding
				, *m_buffer
				, 0u
				, uint32_t( m_size ) );
		}
	}

	void ShaderBuffer::doUpdate( VkDeviceSize offset
		, VkDeviceSize size )
	{
		CU_Require( ( offset + size <= m_size )
			|| ( offset == 0u && size == ashes::WholeSize ) );

		if ( uint8_t * buffer = m_buffer->lock( offset
			, size
			, 0u ) )
		{
			std::memcpy( buffer, m_data.data(), std::min( size, m_size ) );
			m_buffer->flush( offset, size );
			m_buffer->unlock();
		}
	}
}
