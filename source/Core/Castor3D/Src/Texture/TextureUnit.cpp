#include "TextureUnit.hpp"

#include "Engine.hpp"

#include "TextureLayout.hpp"
#include "Sampler.hpp"

#include "Render/RenderTarget.hpp"
#include "Scene/Scene.hpp"

#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	TextureUnit::TextWriter::TextWriter( String const & tabs, MaterialType type )
		: Castor::TextWriter< TextureUnit >{ tabs }
		, m_type{ type }
	{
	}

	bool TextureUnit::TextWriter::operator()( TextureUnit const & unit, TextFile & file )
	{
		bool result = true;

		if ( unit.IsTextured() && unit.GetTexture() )
		{
			auto texture = unit.GetTexture();
			auto image = texture->GetImage().ToString();

			if ( !image.empty() || !texture->GetImage().IsStaticSource() )
			{
				if ( result )
				{
					result = file.WriteText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
							   && file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( result && unit.GetSampler() && unit.GetSampler()->GetName() != cuT( "Default" ) )
				{
					result = file.WriteText( m_tabs + cuT( "\tsampler \"" ) + unit.GetSampler()->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< TextureUnit >::CheckError( result, "TextureUnit sampler" );
				}

				if ( result && unit.GetChannel() != TextureChannel::eUndefined )
				{
					switch ( m_type )
					{
					case MaterialType::eLegacy:
					case MaterialType::ePbrSpecularGlossiness:
						switch ( unit.GetChannel() )
						{
						case TextureChannel::eDiffuse:
							result = file.WriteText( m_tabs + cuT( "\tchannel diffuse\n" ) ) > 0;
							break;

						case TextureChannel::eNormal:
							result = file.WriteText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
							break;

						case TextureChannel::eOpacity:
							result = file.WriteText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
							break;

						case TextureChannel::eSpecular:
							result = file.WriteText( m_tabs + cuT( "\tchannel specular\n" ) ) > 0;
							break;

						case TextureChannel::eEmissive:
							result = file.WriteText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
							break;

						case TextureChannel::eHeight:
							result = file.WriteText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
							break;

						case TextureChannel::eGloss:
							result = file.WriteText( m_tabs + cuT( "\tchannel gloss\n" ) ) > 0;
							break;

						case TextureChannel::eAmbientOcclusion:
							result = file.WriteText( m_tabs + cuT( "\tchannel ambient_occlusion\n" ) ) > 0;
							break;

						default:
							break;
						}
						break;

					case MaterialType::ePbrMetallicRoughness:
						switch ( unit.GetChannel() )
						{
						case TextureChannel::eAlbedo:
							result = file.WriteText( m_tabs + cuT( "\tchannel albedo\n" ) ) > 0;
							break;

						case TextureChannel::eNormal:
							result = file.WriteText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
							break;

						case TextureChannel::eOpacity:
							result = file.WriteText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
							break;

						case TextureChannel::eMetallic:
							result = file.WriteText( m_tabs + cuT( "\tchannel metallic\n" ) ) > 0;
							break;

						case TextureChannel::eEmissive:
							result = file.WriteText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
							break;

						case TextureChannel::eHeight:
							result = file.WriteText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
							break;

						case TextureChannel::eRoughness:
							result = file.WriteText( m_tabs + cuT( "\tchannel roughness\n" ) ) > 0;
							break;

						case TextureChannel::eAmbientOcclusion:
							result = file.WriteText( m_tabs + cuT( "\tchannel ambient_occlusion\n" ) ) > 0;
							break;

						default:
							break;
						}
						break;
					}

					if ( !texture->GetImage().IsStaticSource() )
					{
						if ( result && unit.GetRenderTarget() )
						{
							result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *unit.GetRenderTarget(), file );
						}
					}
					else
					{
						Path relative{ Scene::TextWriter::CopyFile( Path{ image }, file.GetFilePath(), Path{ cuT( "Textures" ) } ) };
						String path = relative;
						string::replace( path, cuT( "\\" ), cuT( "/" ) );

						if ( unit.GetChannel() == TextureChannel::eOpacity )
						{
							result = file.WriteText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\" a\n" ) ) > 0;
						}
						else
						{
							result = file.WriteText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\" rgb\n" ) ) > 0;
						}
						Castor::TextWriter< TextureUnit >::CheckError( result, "TextureUnit image" );
					}

					if ( result )
					{
						result = file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
					}
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_index( 0 )
		, m_channel( TextureChannel::eDiffuse )
		, m_autoMipmaps( false )
		, m_changed( false )
		, m_sampler( engine.GetDefaultSampler() )
	{
		m_transformations.set_identity();
	}

	TextureUnit::~TextureUnit()
	{
		if ( !m_renderTarget.expired() )
		{
			GetEngine()->GetRenderTargetCache().Remove( std::move( m_renderTarget.lock() ) );
		}
	}

	void TextureUnit::SetTexture( TextureLayoutSPtr texture )
	{
		m_texture = texture;
		m_changed = true;
	}

	bool TextureUnit::Initialise()
	{
		RenderTargetSPtr target = m_renderTarget.lock();
		bool result = false;

		if ( target )
		{
			target->Initialise( GetIndex() );
			m_texture = target->GetTexture().GetTexture();
			result = true;
		}
		else if ( m_texture )
		{
			result = m_texture->Initialise();
			auto sampler = GetSampler();

			if ( result
				&& sampler
				&& sampler->GetInterpolationMode( InterpolationFilter::eMip ) != InterpolationMode::eNearest )
			{
				m_texture->Bind( 0u );
				m_texture->GenerateMipmaps();
				m_texture->Unbind( 0u );
			}
		}

		return result;
	}

	void TextureUnit::Cleanup()
	{
		if ( m_texture )
		{
			m_texture->Cleanup();
		}

		RenderTargetSPtr target = m_renderTarget.lock();

		if ( target )
		{
			target->Cleanup();
		}
	}

	void TextureUnit::Bind()const
	{
		if ( m_texture && m_texture->IsInitialised() )
		{
			m_texture->Bind( m_index );

			if ( m_changed
				&& m_autoMipmaps
				&& m_texture->GetType() != TextureType::eBuffer )
			{
				m_texture->GenerateMipmaps();
				m_changed = false;
			}

			auto sampler = GetSampler();

			if ( sampler )
			{
				sampler->Bind( m_index );
			}
		}
	}

	void TextureUnit::Unbind()const
	{
		auto sampler = GetSampler();

		if ( sampler )
		{
			sampler->Unbind( m_index );
		}

		if ( m_texture && m_texture->IsInitialised() )
		{
			m_texture->Unbind( m_index );
		}
	}

	TextureType TextureUnit::GetType()const
	{
		REQUIRE( m_texture );
		return m_texture->GetType();
	}
}
