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
		renderer::BufferBasePtr doGetSsbo( Engine & engine
			, uint32_t size )
		{
			renderer::BufferBasePtr result;

			if ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createBuffer( size
					, renderer::BufferTarget::eStorageBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			return result;
		}

		renderer::UniformBufferBasePtr doGetTbo( Engine & engine
			, uint32_t size
			, bool isTbo )
		{
			renderer::UniformBufferBasePtr result;

			if ( isTbo )
			{
				result = engine.getRenderSystem()->getCurrentDevice()->createUniformBuffer( 1u
					, size
					, renderer::BufferTarget::eUniformTexelBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size )
		: m_ssbo{ doGetSsbo( engine, size ) }
		, m_tbo{ doGetTbo( engine, size, m_ssbo == nullptr ) }
		, m_data( size_t( size ), uint8_t( 0 ) )
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_ssbo.reset();
		m_tbo.reset();
	}

	void ShaderBuffer::update()
	{
		update( 0u, uint32_t( m_data.size() ) );
	}

	void ShaderBuffer::update( uint32_t offset, uint32_t size )
	{
		REQUIRE( size + offset <= m_data.size() );

		if ( m_ssbo )
		{
			if ( uint8_t * buffer = m_ssbo->lock( offset
				, size
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, m_data.data(), size );
				m_ssbo->flush( 0u, size );
				m_ssbo->unlock();
			}
		}
		else
		{
			if ( uint8_t * buffer = m_tbo->getBuffer().lock( offset
				, size
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::memcpy( buffer, m_data.data(), size );
				m_tbo->getBuffer().flush( 0u, size );
				m_tbo->getBuffer().unlock();
			}
		}
	}

	renderer::DescriptorSetLayoutBinding ShaderBuffer::createLayoutBinding( uint32_t index )const
	{
		if ( m_ssbo )
		{
			return { index, renderer::DescriptorType::eStorageBuffer, renderer::ShaderStageFlag::eFragment };
		}
		else
		{
			return { index, renderer::DescriptorType::eUniformTexelBuffer, renderer::ShaderStageFlag::eFragment };
		}
	}

	void ShaderBuffer::createBinding( renderer::DescriptorSet & descriptorSet
		, renderer::DescriptorSetLayoutBinding const & binding )const
	{
		if ( m_ssbo )
		{
			descriptorSet.createBinding( binding
				, *m_ssbo
				, 0u
				, uint32_t( m_data.size() ) );
		}
		else
		{
			descriptorSet.createBinding( binding
				, *m_tbo
				, 0u
				, uint32_t( m_data.size() ) );
		}
	}
}
