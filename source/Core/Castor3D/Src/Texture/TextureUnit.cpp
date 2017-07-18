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
	TextureUnit::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< TextureUnit >{ p_tabs }
	{
	}

	bool TextureUnit::TextWriter::operator()( TextureUnit const & p_unit, TextFile & p_file )
	{
		bool result = true;

		if ( p_unit.IsTextured() && p_unit.GetTexture() )
		{
			auto texture = p_unit.GetTexture();
			auto image = texture->GetImage().ToString();

			if ( !image.empty() || !texture->GetImage().IsStaticSource() )
			{
				if ( result )
				{
					result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
							   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( result && p_unit.GetSampler() && p_unit.GetSampler()->GetName() != cuT( "Default" ) )
				{
					result = p_file.WriteText( m_tabs + cuT( "\tsampler \"" ) + p_unit.GetSampler()->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< TextureUnit >::CheckError( result, "TextureUnit sampler" );
				}

				if ( result && p_unit.GetChannel() != TextureChannel::eUndefined )
				{
					switch ( p_unit.GetChannel() )
					{
					case TextureChannel::eDiffuse:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel diffuse\n" ) ) > 0;
						break;

					case TextureChannel::eNormal:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
						break;

					case TextureChannel::eOpacity:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
						break;

					case TextureChannel::eSpecular:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel specular\n" ) ) > 0;
						break;

					case TextureChannel::eEmissive:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
						break;

					case TextureChannel::eHeight:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
						break;

					case TextureChannel::eGloss:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel gloss\n" ) ) > 0;
						break;

					case TextureChannel::eAmbientOcclusion:
						result = p_file.WriteText( m_tabs + cuT( "\tchannel ambient_occlusion\n" ) ) > 0;
						break;

					default:
						break;
					}

					if ( !texture->GetImage().IsStaticSource() )
					{
						if ( result && p_unit.GetRenderTarget() )
						{
							result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_unit.GetRenderTarget(), p_file );
						}
					}
					else
					{
						Path relative{ Scene::TextWriter::CopyFile( Path{ image }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } ) };
						String path = relative;
						string::replace( path, cuT( "\\" ), cuT( "/" ) );
						result = p_file.WriteText( m_tabs + cuT( "\timage \"" ) + path + cuT( "\"\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( result, "TextureUnit image" );
					}

					if ( result )
					{
						result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
					}
				}
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_index( 0 )
		, m_channel( TextureChannel::eDiffuse )
		, m_autoMipmaps( false )
		, m_changed( false )
		, m_sampler( p_engine.GetDefaultSampler() )
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

	void TextureUnit::SetTexture( TextureLayoutSPtr p_texture )
	{
		m_texture = p_texture;
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
