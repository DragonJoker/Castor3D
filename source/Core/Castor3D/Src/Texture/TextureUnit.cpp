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
		bool l_result = true;

		if ( p_unit.IsTextured() && p_unit.GetTexture() )
		{
			auto l_texture = p_unit.GetTexture();
			auto l_image = l_texture->GetImage().ToString();

			if ( !l_image.empty() || !l_texture->GetImage().IsStaticSource() )
			{
				if ( l_result )
				{
					l_result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
							   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( l_result && p_unit.GetSampler() && p_unit.GetSampler()->GetName() != cuT( "Default" ) )
				{
					l_result = p_file.WriteText( m_tabs + cuT( "\tsampler \"" ) + p_unit.GetSampler()->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< TextureUnit >::CheckError( l_result, "TextureUnit sampler" );
				}

				if ( l_result && p_unit.GetChannel() != TextureChannel::eUndefined )
				{
					switch ( p_unit.GetChannel() )
					{
					case TextureChannel::eDiffuse:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel diffuse\n" ) ) > 0;
						break;

					case TextureChannel::eNormal:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
						break;

					case TextureChannel::eOpacity:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
						break;

					case TextureChannel::eSpecular:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel specular\n" ) ) > 0;
						break;

					case TextureChannel::eEmissive:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
						break;

					case TextureChannel::eHeight:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
						break;

					case TextureChannel::eGloss:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel gloss\n" ) ) > 0;
						break;

					case TextureChannel::eAmbientOcclusion:
						l_result = p_file.WriteText( m_tabs + cuT( "\tchannel ambient_occlusion\n" ) ) > 0;
						break;

					default:
						break;
					}

					if ( !l_texture->GetImage().IsStaticSource() )
					{
						if ( l_result && p_unit.GetRenderTarget() )
						{
							l_result = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_unit.GetRenderTarget(), p_file );
						}
					}
					else
					{
						Path l_relative{ Scene::TextWriter::CopyFile( Path{ l_image }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } ) };
						String l_path = l_relative;
						string::replace( l_path, cuT( "\\" ), cuT( "/" ) );
						l_result = p_file.WriteText( m_tabs + cuT( "\timage \"" ) + l_path + cuT( "\"\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_result, "TextureUnit image" );
					}

					if ( l_result )
					{
						l_result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
					}
				}
			}
		}

		return l_result;
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
		RenderTargetSPtr l_target = m_renderTarget.lock();
		bool l_result = false;

		if ( l_target )
		{
			l_target->Initialise( GetIndex() );
			m_texture = l_target->GetTexture().GetTexture();
			l_result = true;
		}
		else if ( m_texture )
		{
			l_result = m_texture->Initialise();
			auto l_sampler = GetSampler();

			if ( l_result
				&& l_sampler
				&& l_sampler->GetInterpolationMode( InterpolationFilter::eMip ) != InterpolationMode::eNearest )
			{
				m_texture->Bind( 0u );
				m_texture->GenerateMipmaps();
				m_texture->Unbind( 0u );
			}
		}

		return l_result;
	}

	void TextureUnit::Cleanup()
	{
		if ( m_texture )
		{
			m_texture->Cleanup();
		}

		RenderTargetSPtr l_target = m_renderTarget.lock();

		if ( l_target )
		{
			l_target->Cleanup();
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

			auto l_sampler = GetSampler();

			if ( l_sampler )
			{
				l_sampler->Bind( m_index );
			}
		}
	}

	void TextureUnit::Unbind()const
	{
		auto l_sampler = GetSampler();

		if ( l_sampler )
		{
			l_sampler->Unbind( m_index );
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
