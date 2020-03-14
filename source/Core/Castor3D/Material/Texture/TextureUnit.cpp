#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		String writeMask( uint32_t mask )
		{
			auto stream = castor::makeStringStream();
			stream << cuT( "0x" ) << std::hex << std::setw( 8u ) << std::setfill( cuT( '0' ) ) << mask;
			return stream.str();
		}

		void doUpdateShift( castor::Point2ui & mask )
		{
			if ( mask[0] )
			{
				for ( uint32_t i = 0u; i < 4u; ++i )
				{
					if ( mask[0] & ( 0x000000FFu << ( i * 8u ) ) )
					{
						mask[1] = i;
						i = 4u;
					}
				}
			}
		}
	}

	TextureUnit::TextWriter::TextWriter( String const & tabs, MaterialType type )
		: castor::TextWriter< TextureUnit >{ tabs, "TextureUnit" }
		, m_type{ type }
	{
	}

	bool TextureUnit::TextWriter::operator()( TextureUnit const & unit, TextFile & file )
	{
		bool result = true;

		if ( unit.isTextured() && unit.getTexture() )
		{
			auto texture = unit.getTexture();
			auto image = texture->getDefaultImage().toString();

			if ( !image.empty() || !texture->getDefaultImage().isStaticSource() )
			{
				if ( result )
				{
					if ( auto block = writeHeader( cuT( "texture_unit" ), file ) )
					{
						if ( result && unit.getSampler() && unit.getSampler()->getName() != cuT( "Default" ) )
						{
							result = writeName( cuT( "sampler" ), unit.getSampler()->getName(), file );
						}

						if ( result && unit.getTexture()->getMipmapCount() > 1 )
						{
							result = write( cuT( "levels_count" ), unit.getTexture()->getMipmapCount(), file );
						}

						if ( result )
						{
							if ( !texture->getDefaultImage().isStaticSource() )
							{
								if ( unit.getRenderTarget() )
								{
									result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *unit.getRenderTarget(), file );
									checkError( result, "render_target" );
								}
								else
								{
									// Procedurally generated textures, certainly will go here
								}
							}
							else
							{
								result = writeFile( cuT( "image" ), Path{ image }, cuT( "Textures" ), file );
							}
						}

						if ( result )
						{
							result = TextureConfiguration::TextWriter{ m_tabs, m_type }( unit.getConfiguration(), file );
							checkError( result, "configuration" );
						}
					}
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_autoMipmaps{ false }
		, m_changed{ false }
		, m_sampler{ engine.getDefaultSampler() }
		, m_descriptor
		{
			0u,
			0u,
			1u,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		}
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			getEngine()->getRenderTargetCache().remove( std::move( renderTarget ) );
		}
	}

	void TextureUnit::setTexture( TextureLayoutSPtr texture )
	{
		m_texture = texture;
		m_changed = true;
	}

	bool TextureUnit::initialise()
	{
		RenderTargetSPtr target = m_renderTarget.lock();
		bool result = false;

		if ( target )
		{
			target->initialise();
			m_texture = target->getTexture().getTexture();
			result = true;
		}
		else if ( m_texture )
		{
			result = m_texture->initialise();
			auto sampler = getSampler();
			CU_Require( sampler );
			sampler->initialise();

			if ( result && m_texture->getMipmapCount() > 1u )
			{
				m_texture->generateMipmaps();
			}

			m_descriptor = ashes::WriteDescriptorSet
			{
				0u,
				0u,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				{
					{
						sampler->getSampler(),
						m_texture->getDefaultView(),
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					}
				}
			};
		}

		return result;
	}

	void TextureUnit::cleanup()
	{
		auto sampler = getSampler();

		if ( sampler )
		{
			sampler->cleanup();
		}
		
		if ( m_texture )
		{
			m_texture->cleanup();
		}

		RenderTargetSPtr target = m_renderTarget.lock();

		if ( target )
		{
			target->cleanup();
		}
	}

	VkImageType TextureUnit::getType()const
	{
		CU_Require( m_texture );
		return m_texture->getType();
	}

	castor::String TextureUnit::toString()const
	{
		auto renderTarget = m_renderTarget.lock();

		if ( renderTarget )
		{
			return cuT( "RT_" ) + castor::string::toString( renderTarget->getIndex() );
		}

		if ( m_configuration.environment )
		{
			return cuT( "EnvMap_" ) + castor::string::toString( m_configuration.environment );
		}

		CU_Require( m_texture );
		return m_texture->getDefaultImage().toString();
	}

	void TextureUnit::setConfiguration( TextureConfiguration value )
	{
		m_configuration = std::move( value );
		m_configuration.needsYInversion = m_texture
			? ( m_texture->needsYInversion()
				? 1u
				: 0u )
			: 0u;
		doUpdateShift( m_configuration.colourMask );
		doUpdateShift( m_configuration.specularMask );
		doUpdateShift( m_configuration.glossinessMask );
		doUpdateShift( m_configuration.opacityMask );
		doUpdateShift( m_configuration.emissiveMask );
		doUpdateShift( m_configuration.normalMask );
		doUpdateShift( m_configuration.heightMask );
		doUpdateShift( m_configuration.occlusionMask );
		doUpdateShift( m_configuration.transmittanceMask );
		onChanged( *this );
	}
}
