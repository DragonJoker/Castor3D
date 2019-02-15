#include "Castor3D/Texture/TextureUnit.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	TextureUnit::TextWriter::TextWriter( String const & tabs, MaterialType type )
		: castor::TextWriter< TextureUnit >{ tabs }
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
					result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( result && unit.getSampler() && unit.getSampler()->getName() != cuT( "Default" ) )
				{
					result = file.writeText( m_tabs + cuT( "\tsampler \"" ) + unit.getSampler()->getName() + cuT( "\"\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit sampler" );
				}

				if ( result && unit.getTexture()->getMipmapCount() > 1 )
				{
					result = file.writeText( m_tabs + cuT( "\tlevels_count " ) + string::toString( unit.getTexture()->getMipmapCount(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
					castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit mip level count" );
				}

				if ( result && unit.getChannel() != TextureChannel::eUndefined )
				{
					switch ( m_type )
					{
					case MaterialType::eLegacy:
					case MaterialType::ePbrSpecularGlossiness:
						switch ( unit.getChannel() )
						{
						case TextureChannel::eDiffuse:
							result = file.writeText( m_tabs + cuT( "\tchannel diffuse\n" ) ) > 0;
							break;

						case TextureChannel::eSpecular:
							result = file.writeText( m_tabs + cuT( "\tchannel specular\n" ) ) > 0;
							break;

						case TextureChannel::eGloss:
							result = file.writeText( m_tabs + cuT( "\tchannel gloss\n" ) ) > 0;
							break;

						default:
							break;
						}
						break;

					case MaterialType::ePbrMetallicRoughness:
						switch ( unit.getChannel() )
						{
						case TextureChannel::eAlbedo:
							result = file.writeText( m_tabs + cuT( "\tchannel albedo\n" ) ) > 0;
							break;

						case TextureChannel::eMetallic:
							result = file.writeText( m_tabs + cuT( "\tchannel metallic\n" ) ) > 0;
							break;

						case TextureChannel::eRoughness:
							result = file.writeText( m_tabs + cuT( "\tchannel roughness\n" ) ) > 0;
							break;

						default:
							break;
						}
						break;
					}

					switch ( unit.getChannel() )
					{
					case TextureChannel::eNormal:
						result = file.writeText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
						break;

					case TextureChannel::eOpacity:
						result = file.writeText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
						break;

					case TextureChannel::eEmissive:
						result = file.writeText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
						break;

					case TextureChannel::eHeight:
						result = file.writeText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
						break;

					case TextureChannel::eAmbientOcclusion:
						result = file.writeText( m_tabs + cuT( "\tchannel ambient_occlusion\n" ) ) > 0;
						break;

					case TextureChannel::eTransmittance:
						result = file.writeText( m_tabs + cuT( "\tchannel transmittance\n" ) ) > 0;
						break;
					}

					if ( !texture->getDefaultImage().isStaticSource() )
					{
						if ( result && unit.getRenderTarget() )
						{
							result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *unit.getRenderTarget(), file );
						}
					}
					else
					{
						Path relative{ Scene::TextWriter::copyFile( Path{ image }, file.getFilePath(), Path{ cuT( "Textures" ) } ) };
						String path = relative;
						string::replace( path, cuT( "\\" ), cuT( "/" ) );

						if ( unit.getChannel() == TextureChannel::eOpacity )
						{
							result = file.writeText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\" a\n" ) ) > 0;
						}
						else
						{
							result = file.writeText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\" rgb\n" ) ) > 0;
						}

						castor::TextWriter< TextureUnit >::checkError( result, "TextureUnit image" );
					}

					if ( result )
					{
						result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
					}
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_channel( TextureChannel::eDiffuse )
		, m_autoMipmaps( false )
		, m_changed( false )
		, m_sampler( engine.getDefaultSampler() )
	{
		m_transformations.setIdentity();
	}

	TextureUnit::~TextureUnit()
	{
		if ( !m_renderTarget.expired() )
		{
			getEngine()->getRenderTargetCache().remove( std::move( m_renderTarget.lock() ) );
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
				1u,
				ashes::DescriptorType::eCombinedImageSampler,
				{
					{
						std::ref( sampler->getSampler() ),
						std::ref( m_texture->getDefaultView() ),
						ashes::ImageLayout::eShaderReadOnlyOptimal,
					}
				}
			};
		}

		return result;
	}

	void TextureUnit::cleanup()
	{
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

	ashes::TextureType TextureUnit::getType()const
	{
		CU_Require( m_texture );
		return m_texture->getType();
	}
}
