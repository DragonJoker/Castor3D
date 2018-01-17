#include "ShaderBuffer.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/Sampler.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		ShaderStorageBufferUPtr doGetSsbo( Engine & engine
			, uint32_t size )
		{
			ShaderStorageBufferUPtr ssbo;

			if ( engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eShaderStorageBuffers ) )
			{
				ssbo = std::make_unique< ShaderStorageBuffer >( engine );
				ssbo->resize( size );
				ssbo->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			}

			return ssbo;
		}

		TextureUnit doGetTbo( Engine & engine
			, uint32_t size
			, ShaderStorageBuffer * ssbo )
		{
			TextureUnit tbo{ engine };

			if ( !ssbo )
			{
				auto texture = engine.getRenderSystem()->createTexture( TextureType::eBuffer
					, AccessType::eWrite
					, AccessType::eRead
					, PixelFormat::eRGBA32F
					, Size( size / PixelDefinitions< PixelFormat::eRGBA32F >::Size, 1 ) );
				texture->getImage().initialiseSource();
				auto sampler = engine.getLightsSampler();
				tbo.setAutoMipmaps( false );
				tbo.setSampler( sampler );
				tbo.setTexture( texture );
				tbo.setIndex( 0u );
				tbo.initialise();
			}

			return tbo;
		}

		castor::PxBufferBaseSPtr doGetBuffer( TextureUnit & tbo )
		{
			castor::PxBufferBaseSPtr result;

			if ( tbo.getTexture() )
			{
				result = tbo.getTexture()->getImage().getBuffer();
			}

			return result;
		}
	}

	//*********************************************************************************************

	ShaderBuffer::ShaderBuffer( Engine & engine
		, uint32_t size )
		: m_ssbo{ doGetSsbo( engine, size ) }
		, m_tbo{ doGetTbo( engine, size, m_ssbo.get() ) }
		, m_buffer{ doGetBuffer( m_tbo ) }
	{
	}

	ShaderBuffer::~ShaderBuffer()
	{
		m_tbo.cleanup();

		if ( m_ssbo )
		{
			m_ssbo->cleanup();
			m_ssbo.reset();
		}
	}

	void ShaderBuffer::update()
	{
		if ( m_ssbo )
		{
			m_ssbo->upload();
		}
		else
		{
			auto layout = m_tbo.getTexture();
			REQUIRE( layout );
			auto locked = layout->lock( AccessType::eWrite );

			if ( locked )
			{
				memcpy( locked
					, m_buffer->constPtr()
					, m_buffer->size() );
			}

			layout->unlock( true );
		}
	}

	void ShaderBuffer::bind( uint32_t index )const
	{
		if ( m_ssbo )
		{
			m_ssbo->bindTo( index );
		}
		else
		{
			m_tbo.getTexture()->bind( index );
			m_tbo.getSampler()->bind( index );
		}
	}

	uint8_t * ShaderBuffer::ptr()
	{
		uint8_t * result{ nullptr };

		if ( m_ssbo )
		{
			result = m_ssbo->getData();
		}
		else
		{
			REQUIRE( m_buffer );
			result = m_buffer->ptr();
		}

		return result;
	}
}
