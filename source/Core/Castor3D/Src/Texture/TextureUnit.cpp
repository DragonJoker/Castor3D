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
		static std::map< BlendSource, String > l_strTextureArguments
		{
			{ BlendSource::eTexture, cuT( "texture" ) },
			{ BlendSource::eTexture0, cuT( "texture0" ) },
			{ BlendSource::eTexture1, cuT( "texture1" ) },
			{ BlendSource::eTexture2, cuT( "texture2" ) },
			{ BlendSource::eTexture3, cuT( "texture3" ) },
			{ BlendSource::eDiffuse, cuT( "diffuse" ) },
			{ BlendSource::ePrevious, cuT( "previous" ) },
			{ BlendSource::eConstant, cuT( "constant" ) },
		};
		static std::map< TextureBlendFunc, String > l_strTextureBlendFunctions
		{
			{ TextureBlendFunc::eNoBlend, cuT( "none" ) },
			{ TextureBlendFunc::eFirstArg, cuT( "first_arg" ) },
			{ TextureBlendFunc::eAdd, cuT( "add" ) },
			{ TextureBlendFunc::eAddSigned, cuT( "add_signed" ) },
			{ TextureBlendFunc::eModulate, cuT( "modulate" ) },
			{ TextureBlendFunc::eInterpolate, cuT( "interpolate" ) },
			{ TextureBlendFunc::eSubtract, cuT( "substract" ) },
		};
		static std::map< ComparisonFunc, String > l_strAlphaFuncs
		{
			{ ComparisonFunc::eAlways, cuT( "always" ) },
			{ ComparisonFunc::eLess, cuT( "less" ) },
			{ ComparisonFunc::eLEqual, cuT( "less_or_equal" ) },
			{ ComparisonFunc::eEqual, cuT( "equal" ) },
			{ ComparisonFunc::eNEqual, cuT( "not_equal" ) },
			{ ComparisonFunc::eGEqual, cuT( "greater_or_equal" ) },
			{ ComparisonFunc::eGreater, cuT( "greater" ) },
			{ ComparisonFunc::eNever, cuT( "never" ) },
		};
		bool l_return = true;

		if ( p_unit.IsTextured() && p_unit.GetTexture() )
		{
			auto l_texture = p_unit.GetTexture();
			auto l_image = l_texture->GetImage().ToString();

			if ( !l_image.empty() || !l_texture->GetImage().IsStaticSource() )
			{
				if ( l_return )
				{
					l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "texture_unit\n" ) ) > 0
							   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
				}

				if ( l_return )
				{
					l_return = p_file.Print( 256, cuT( "%s\tcolour " ), m_tabs.c_str() ) > 0
							   && Colour::TextWriter( String() )( p_unit.GetBlendColour(), p_file )
							   && p_file.Print( 256, cuT( "\n" ) ) > 0;
					Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit colour" );
				}

				if ( l_return && p_unit.GetSampler() && p_unit.GetSampler()->GetName() != cuT( "Default" ) )
				{
					l_return = p_file.WriteText( m_tabs + cuT( "\tsampler \"" ) + p_unit.GetSampler()->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit sampler" );
				}

				if ( l_return && p_unit.GetChannel() != TextureChannel::eUndefined )
				{
					switch ( p_unit.GetChannel() )
					{
					case TextureChannel::eDiffuse:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel diffuse\n" ) ) > 0;
						break;

					case TextureChannel::eNormal:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel normal\n" ) ) > 0;
						break;

					case TextureChannel::eOpacity:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel opacity\n" ) ) > 0;
						break;

					case TextureChannel::eSpecular:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel specular\n" ) ) > 0;
						break;

					case TextureChannel::eEmissive:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel emissive\n" ) ) > 0;
						break;

					case TextureChannel::eHeight:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel height\n" ) ) > 0;
						break;

					case TextureChannel::eGloss:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel gloss\n" ) ) > 0;
						break;

					default:
						break;
					}

					Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit channel" );

					if ( l_return && p_unit.GetAlphaFunc() != ComparisonFunc::eAlways )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "\talpha_func " )
							+ l_strAlphaFuncs[p_unit.GetAlphaFunc()] + cuT( " " )
							+ string::to_string( p_unit.GetAlphaValue() ) + cuT( "\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit alpha function" );
					}

					if ( l_return && p_unit.GetRgbBlendMode() != TextureBlendFunc::eNoBlend )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "\trgb_blend " )
							+ l_strTextureBlendFunctions[p_unit.GetRgbBlendMode()] + cuT( " " )
							+ l_strTextureArguments[p_unit.GetRgbBlendArgument( BlendSrcIndex::eIndex0 )] + cuT( " " )
							+ l_strTextureArguments[p_unit.GetRgbBlendArgument( BlendSrcIndex::eIndex1 )] + cuT( "\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit rgb blend" );
					}

					if ( l_return && p_unit.GetAlphaBlendMode() != TextureBlendFunc::eNoBlend )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "\talpha_blend " )
							+ l_strTextureBlendFunctions[p_unit.GetAlphaBlendMode()] + cuT( " " )
							+ l_strTextureArguments[p_unit.GetAlphaBlendArgument( BlendSrcIndex::eIndex0 )] + cuT( " " )
							+ l_strTextureArguments[p_unit.GetAlphaBlendArgument( BlendSrcIndex::eIndex1 )] + cuT( "\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit alpha blend" );
					}

					if ( !l_texture->GetImage().IsStaticSource() )
					{
						if ( l_return && p_unit.GetRenderTarget() )
						{
							l_return = RenderTarget::TextWriter( m_tabs + cuT( "\t" ) )( *p_unit.GetRenderTarget(), p_file );
						}
					}
					else
					{
						Path l_relative{ Scene::TextWriter::CopyFile( Path{ l_image }, p_file.GetFilePath(), Path{ cuT( "Textures" ) } ) };
						String l_path = l_relative;
						string::replace( l_path, cuT( "\\" ), cuT( "/" ) );
						l_return = p_file.WriteText( m_tabs + cuT( "\timage \"" ) + l_path + cuT( "\"\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit image" );
					}

					if ( l_return )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
					}
				}
			}
		}

		return l_return;
	}

	//*********************************************************************************************

	TextureUnit::TextureUnit( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
		, m_index( 0 )
		, m_blendColour( Colour::from_rgba( 0xFFFFFFFF ) )
		, m_channel( TextureChannel::eDiffuse )
		, m_alphaFunc( ComparisonFunc::eAlways )
		, m_alphaValue( 0 )
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
		bool l_return = false;

		if ( l_target )
		{
			l_target->Initialise( GetIndex() );
			m_texture = l_target->GetTexture().GetTexture();
			l_return = true;
		}
		else if ( m_texture )
		{
			l_return = m_texture->Initialise();
		}

		return l_return;
	}

	void TextureUnit::Cleanup()
	{
		if ( m_texture )
		{
			m_texture->Cleanup();
		}

		m_blendColour = Colour();
		RenderTargetSPtr l_target = m_renderTarget.lock();

		if ( l_target )
		{
			l_target->Cleanup();
		}

		m_alphaValue = 1.0f;
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
