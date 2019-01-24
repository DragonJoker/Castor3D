#include "ShaderBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/Buffer.hpp>
#include <Buffer/UniformBuffer.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ashes::BufferBasePtr doCreateBuffer( Engine & engine
			, uint32_t size
			, bool forceTbo )
		{
			ashes::BufferBasePtr result;
			ashes::BufferTarget target = forceTbo
				? ashes::BufferTarget::eUniformTexelBuffer
				: ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
					? ashes::BufferTarget::eStorageBuffer
					: ashes::BufferTarget::eUniformTexelBuffer );
			result = getCurrentDevice( engine ).createBuffer( size
				, target | ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
			return result;
		}

		ashes::BufferViewPtr doCreateView( Engine & engine
			, uint32_t size
			, bool forceTbo
			, ashes::BufferBase const & buffer )
		{
			ashes::BufferViewPtr result;

			if ( forceTbo || !engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = getCurrentDevice( engine ).createBufferView( buffer
					, ashes::Format::eR32G32B32A32_SFLOAT
					, 0u
					, uint32_t( buffer.getSize() ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size
		, bool forceTbo )
		: m_buffer{ doCreateBuffer( engine, size, forceTbo ) }
		, m_bufferView{ doCreateView( engine, size, forceTbo, *m_buffer ) }
		, m_data( size_t( size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_bufferView.reset();
		m_buffer.reset();
	}

	void ShaderBuffer::update()
	{
		update( 0u, uint32_t( m_data.size() ) );
	}

	void ShaderBuffer::update( uint32_t offset, uint32_t size )
	{
		CU_Require( size + offset <= m_data.size() );
		if ( uint8_t * buffer = m_buffer->lock( offset
			, size
			, ashes::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, m_data.data(), size );
			m_buffer->flush( 0u, size );
			m_buffer->unlock();
		}
	}

	ashes::DescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		if ( m_bufferView )
		{
			return { index, ashes::DescriptorType::eUniformTexelBuffer, ashes::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, ashes::DescriptorType::eStorageBuffer, ashes::ShaderStageFlag::eFragment };
		}
	}

	void ShaderBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, ashes::DescriptorSetLayoutBinding const & binding )const
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
				, uint32_t( m_data.size() ) );
		}
	}
}
