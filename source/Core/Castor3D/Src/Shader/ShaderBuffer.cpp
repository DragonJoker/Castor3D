#include "ShaderBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/Buffer.hpp>
#include <Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		renderer::BufferBasePtr doGetStorage( Engine & engine
			, uint32_t size )
		{
			renderer::BufferBasePtr result;

			if ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createBuffer( size
					, renderer::BufferTarget::eStorageBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}
			else
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createBuffer( size
					, renderer::BufferTarget::eUniformTexelBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size )
		: m_buffer{ doGetStorage( engine, size ) }
		, m_data( size_t( size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_buffer.reset();
	}

	void ShaderBuffer::update()
	{
		if ( uint8_t * buffer = m_buffer->lock( 0u
			, uint32_t( m_data.size() )
			, renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( buffer, m_data.data(), m_data.size() );
			m_buffer->unlock( uint32_t( m_data.size() ), true );
		}
	}

	renderer::DescriptorSetLayoutBinding ShaderBuffer::createBinding( uint32_t index )const
	{
		if ( checkFlag( m_buffer->getTargets(), renderer::BufferTarget::eStorageBuffer ) )
		{
			return { index, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, renderer::DescriptorType::eUniformTexelBuffer, renderer::ShaderStageFlag::eFragment };
		}
	}
}
