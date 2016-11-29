#include "TextureUnit.hpp"

#include "Engine.hpp"
#include "TargetCache.hpp"

#include "TextureLayout.hpp"
#include "Sampler.hpp"
#include "TextureImage.hpp"
#include "TextureLayout.hpp"

#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Scene.hpp"
#include "Technique/RenderTechnique.hpp"

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
		static std::map< AlphaBlendFunc, String > l_strTextureAlphaFunctions
		{
			{ AlphaBlendFunc::eNoBlend, cuT( "none" ) },
			{ AlphaBlendFunc::eFirstArg, cuT( "first_arg" ) },
			{ AlphaBlendFunc::eAdd, cuT( "add" ) },
			{ AlphaBlendFunc::eAddSigned, cuT( "add_signed" ) },
			{ AlphaBlendFunc::eModulate, cuT( "modulate" ) },
			{ AlphaBlendFunc::eInterpolate, cuT( "interpolate" ) },
			{ AlphaBlendFunc::eSubtract, cuT( "substract" ) },
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
		static std::map< ColourBlendFunc, String > l_strTextureRgbFunctions
		{
			{ ColourBlendFunc::eNoBlend, cuT( "none" ) },
			{ ColourBlendFunc::eFirstArg, cuT( "first_arg" ) },
			{ ColourBlendFunc::eAdd, cuT( "add" ) },
			{ ColourBlendFunc::eAddSigned, cuT( "add_signed" ) },
			{ ColourBlendFunc::eModulate, cuT( "modulate" ) },
			{ ColourBlendFunc::eInterpolate, cuT( "interpolate" ) },
			{ ColourBlendFunc::eSubtract, cuT( "substract" ) },
			{ ColourBlendFunc::eDot3RGB, cuT( "dot3_rgb" ) },
			{ ColourBlendFunc::eDot3RGBA, cuT( "dot3_rgba" ) },
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
					case TextureChannel::eColour:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel colour\n" ) ) > 0;
						break;

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

					case TextureChannel::eAmbient:
						l_return = p_file.WriteText( m_tabs + cuT( "\tchannel ambient\n" ) ) > 0;
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
						l_return = p_file.WriteText( m_tabs + cuT( "\talpha_func " ) + l_strAlphaFuncs[p_unit.GetAlphaFunc()] + cuT( " " ) + string::to_string( p_unit.GetAlphaValue() ) + cuT( "\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit alpha function" );
					}

					if ( l_return && p_unit.GetRgbFunction() != ColourBlendFunc::eNoBlend )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "\trgb_blend " ) + l_strTextureRgbFunctions[p_unit.GetRgbFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( BlendSrcIndex::eIndex0 )] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( BlendSrcIndex::eIndex1 )] + cuT( "\n" ) ) > 0;
						Castor::TextWriter< TextureUnit >::CheckError( l_return, "TextureUnit rgb blend" );
					}

					if ( l_return && p_unit.GetAlpFunction() != AlphaBlendFunc::eNoBlend )
					{
						l_return = p_file.WriteText( m_tabs + cuT( "\talpha_blend " ) + l_strTextureAlphaFunctions[p_unit.GetAlpFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetAlpArgument( BlendSrcIndex::eIndex0 )] + cuT( " " ) + l_strTextureArguments[p_unit.GetAlpArgument( BlendSrcIndex::eIndex1 )] + cuT( "\n" ) ) > 0;
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
						l_return = p_file.WriteText( m_tabs + cuT( "\timage \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
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
		, m_clrBlend( Colour::from_rgba( 0xFFFFFFFF ) )
		, m_eChannel( TextureChannel::eDiffuse )
		, m_eAlphaFunc( ComparisonFunc::eAlways )
		, m_fAlphaValue( 0 )
		, m_eRgbFunction( ColourBlendFunc::eNoBlend )
		, m_eAlpFunction( AlphaBlendFunc::eNoBlend )
		, m_bAutoMipmaps( false )
		, m_changed( false )
		, m_pSampler( p_engine.GetDefaultSampler() )
	{
		m_mtxTransformations.set_identity();
		m_eRgbArguments[0] = BlendSource::eCount;
		m_eRgbArguments[1] = BlendSource::eCount;
		m_eAlpArguments[0] = BlendSource::eCount;
		m_eAlpArguments[1] = BlendSource::eCount;
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
		m_pTexture = p_texture;
		m_changed = true;
	}

	bool TextureUnit::Initialise()
	{
		RenderTargetSPtr l_target = m_renderTarget.lock();
		bool l_return = false;

		if ( l_target )
		{
			l_target->Initialise( GetIndex() );
			m_pTexture = l_target->GetTexture().GetTexture();
			l_return = true;
		}
		else if ( m_pTexture )
		{
			l_return = m_pTexture->Initialise();
		}

		return l_return;
	}

	void TextureUnit::Cleanup()
	{
		if ( m_pTexture )
		{
			m_pTexture->Cleanup();
		}

		m_clrBlend = Colour();
		RenderTargetSPtr l_target = m_renderTarget.lock();

		if ( l_target )
		{
			l_target->Cleanup();
		}

		m_fAlphaValue = 1.0f;
	}

	void TextureUnit::Bind()const
	{
		if ( m_pTexture && m_pTexture->IsInitialised() )
		{
			auto l_return = m_pTexture->Bind( m_index );

			if ( l_return && m_changed && m_bAutoMipmaps && m_pTexture->GetType() != TextureType::eBuffer )
			{
				m_pTexture->GenerateMipmaps();
				m_changed = false;
			}

			if ( l_return && GetSampler() )
			{
				l_return = GetSampler()->Bind( m_index );
			}
		}
	}

	void TextureUnit::Unbind()const
	{
		if ( GetSampler() )
		{
			GetSampler()->Unbind( m_index );
		}

		if ( m_pTexture && m_pTexture->IsInitialised() )
		{
			m_pTexture->Unbind( m_index );
		}
	}

	bool TextureUnit::IsTextureInitialised()const
	{
		return m_pTexture && m_pTexture->IsInitialised();
	}

	TextureType TextureUnit::GetType()const
	{
		REQUIRE( m_pTexture );
		return m_pTexture->GetType();
	}
}
