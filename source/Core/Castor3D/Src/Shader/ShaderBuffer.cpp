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
		renderer::BufferBasePtr doCreateBuffer( Engine & engine
			, uint32_t size )
		{
			renderer::BufferBasePtr result;
			renderer::BufferTarget target = engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers )
				? renderer::BufferTarget::eStorageBuffer
				: renderer::BufferTarget::eUniformTexelBuffer;
			result = getCurrentDevice( engine ).createBuffer( size
				, target | renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
			return result;
		}

		renderer::BufferViewPtr doCreateView( Engine & engine
			, uint32_t size
			, renderer::BufferBase const & buffer )
		{
			renderer::BufferViewPtr result;

			if ( !engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = getCurrentDevice( engine ).createBufferView( buffer
					, renderer::Format::eR32G32B32A32_SFLOAT
					, 0u
					, uint32_t( buffer.getSize() ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size )
		: m_buffer{ doCreateBuffer( engine, size ) }
		, m_bufferView{ doCreateView( engine, size, *m_buffer ) }
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
		REQUIRE( size + offset <= m_data.size() );
		if ( uint8_t * buffer = m_buffer->lock( offset
			, size
			, renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, m_data.data(), size );
			m_buffer->flush( 0u, size );
			m_buffer->unlock();
		}
	}

	renderer::DescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		if ( m_bufferView )
		{
			return { index, renderer::DescriptorType::eUniformTexelBuffer, renderer::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eFragment };
		}
	}

	void ShaderBuffer::createBinding( renderer::DescriptorSet & descriptorSet
		, renderer::DescriptorSetLayoutBinding const & binding )const
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
