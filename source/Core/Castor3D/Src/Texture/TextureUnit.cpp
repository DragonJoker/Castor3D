#include "TextureUnit.hpp"

#include "Engine.hpp"
#include "TargetManager.hpp"

#include "TextureLayout.hpp"
#include "Sampler.hpp"
#include "TextureImage.hpp"
#include "TextureLayout.hpp"

#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Technique/RenderTechnique.hpp"

#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	TextureUnit::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		:	Loader< TextureUnit, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool TextureUnit::TextLoader::operator()( TextureUnit const & p_unit, TextFile & p_file )
	{
		static const String l_strTextureArguments[eBLEND_SOURCE_COUNT] = { cuT( "diffuse" ), cuT( "texture" ), cuT( "previous" ), cuT( "constant" ) };
		static const String l_strTextureAlphaFunctions[eALPHA_BLEND_FUNC_COUNT] = { cuT( "none" ), cuT( "first_arg" ), cuT( "add" ), cuT( "add_signed" ), cuT( "modulate" ), cuT( "interpolate" ), cuT( "substract" ) };
		static const String l_strAlphaFuncs[eALPHA_FUNC_COUNT] = { cuT( "always" ), cuT( "less" ), cuT( "less_or_equal" ), cuT( "equal" ), cuT( "not_equal" ), cuT( "greater_or_equal" ), cuT( "greater" ), cuT( "never" ) };
		static const String l_strTextureRgbFunctions[eRGB_BLEND_FUNC_COUNT] = { cuT( "none" ), cuT( "first_arg" ), cuT( "add" ), cuT( "add_signed" ), cuT( "modulate" ), cuT( "interpolate" ), cuT( "substract" ), cuT( "dot3_rgb" ), cuT( "dot3_rgba" ) };
		static const String l_strFormats[ePIXEL_FORMAT_COUNT] = { cuT( "l4" ), cuT( "a4l4" ), cuT( "l8" ), cuT( "a8l8" ), cuT( "a1r5g5b5" ), cuT( "a4r4g4b4" ), cuT( "r8g8b8" ), cuT( "a8r8g8b8" ), cuT( "dxtc1" ), cuT( "dxtc3" ), cuT( "dxtc5" ), cuT( "yuy2" ), cuT( "depth16" ), cuT( "depth24" ), cuT( "depth32" ) };
		bool l_return = true;

		if ( p_unit.IsTextured() && p_unit.GetTexture() )
		{
			auto l_texture = p_unit.GetTexture();

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\t\ttexture_unit\n\t\t{\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tcolour " ) ) > 0 && Colour::TextLoader()( p_unit.GetBlendColour(), p_file ) && p_file.Print( 256, cuT( "\n" ) ) > 0;
			}

			if ( l_return && p_unit.GetSampler() && p_unit.GetSampler()->GetName() != cuT( "Default" ) )
			{
				l_return = p_file.WriteText( cuT( "\t\t\tsampler \"" ) + p_unit.GetSampler()->GetName() + cuT( "\"\n" ) ) > 0;
			}

			if ( l_return && p_unit.GetChannel() != 0 )
			{
				switch ( p_unit.GetChannel() )
				{
				case eTEXTURE_CHANNEL_COLOUR	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel colour\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_DIFFUSE	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel diffuse\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_NORMAL	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel normal\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_OPACITY	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel opacity\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_SPECULAR	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel specular\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_EMISSIVE	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel emissive\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_HEIGHT	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel height\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_AMBIENT	:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel ambient\n" ) ) > 0;
					break;

				case eTEXTURE_CHANNEL_GLOSS		:
					l_return = p_file.WriteText( cuT( "\t\t\tchannel gloss\n" ) ) > 0;
					break;

				default:
					break;
				}

				if ( l_return && p_unit.GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
				{
					l_return = p_file.WriteText( cuT( "\t\t\talpha_func " ) + l_strAlphaFuncs[p_unit.GetAlphaFunc()] + cuT( " " ) + string::to_string( p_unit.GetAlphaValue() ) + cuT( "\n" ) ) > 0;
				}

				if ( l_return && p_unit.GetRgbFunction() != eRGB_BLEND_FUNC_NONE )
				{
					l_return = p_file.WriteText( cuT( "\t\t\trgb_blend " ) + l_strTextureRgbFunctions[p_unit.GetRgbFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( eBLEND_SRC_INDEX_0 )] + cuT( " " ) + l_strTextureArguments[p_unit.GetRgbArgument( eBLEND_SRC_INDEX_1 )] + cuT( "\n" ) ) > 0;
				}

				if ( l_return && p_unit.GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
				{
					l_return = p_file.WriteText( cuT( "\t\t\talpha_blend " ) + l_strTextureAlphaFunctions[p_unit.GetAlpFunction()] + cuT( " " ) + l_strTextureArguments[p_unit.GetAlpArgument( eBLEND_SRC_INDEX_0 )] + cuT( " " ) + l_strTextureArguments[p_unit.GetAlpArgument( eBLEND_SRC_INDEX_1 )] + cuT( "\n" ) ) > 0;
				}

				if ( !l_texture->GetImage().IsStaticSource() )
				{
					if ( l_return && p_unit.GetRenderTarget() )
					{
						l_return = RenderTarget::TextLoader( cuT( "\t\t\t" ) )( *p_unit.GetRenderTarget(), p_file );
					}
				}
				else
				{
					Path l_path = p_unit.GetTexturePath();
					Path l_relative = Path( cuT( "Textures" ) ) / l_path.GetFileName() + cuT( "." ) + l_path.GetExtension();
					Path l_newPath = p_file.GetFilePath() / l_relative;

					if ( !File::DirectoryExists( l_newPath.GetPath() ) )
					{
						File::DirectoryCreate( l_newPath.GetPath() );
					}

					File::CopyFile( l_path, p_file.GetFilePath() / cuT( "Textures" ) );
					l_return = p_file.WriteText( cuT( "\t\t\timage \"" ) + l_relative + cuT( "\"\n" ) ) > 0;
				}

				if ( l_return )
				{
					l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
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
		, m_eChannel( eTEXTURE_CHANNEL_DIFFUSE )
		, m_eAlphaFunc( eALPHA_FUNC_ALWAYS )
		, m_fAlphaValue( 0 )
		, m_eRgbFunction( eRGB_BLEND_FUNC_NONE )
		, m_eAlpFunction( eALPHA_BLEND_FUNC_NONE )
		, m_bAutoMipmaps( false )
		, m_changed( false )
		, m_pSampler( p_engine.GetDefaultSampler() )
	{
		m_mtxTransformations.set_identity();
		m_eRgbArguments[0] = eBLEND_SOURCE_COUNT;
		m_eRgbArguments[1] = eBLEND_SOURCE_COUNT;
		m_eAlpArguments[0] = eBLEND_SOURCE_COUNT;
		m_eAlpArguments[1] = eBLEND_SOURCE_COUNT;
	}

	TextureUnit::~TextureUnit()
	{
		if ( !m_renderTarget.expired() )
		{
			GetEngine()->GetTargetManager().Remove( std::move( m_renderTarget.lock() ) );
		}
	}

	void TextureUnit::SetTexture( TextureLayoutSPtr p_texture )
	{
		m_pTexture = p_texture;
		m_changed = true;
	}

	void TextureUnit::Initialise()
	{
		RenderTargetSPtr l_target = m_renderTarget.lock();

		if ( l_target )
		{
			l_target->Initialise( GetIndex() );
			m_pTexture = l_target->GetTexture().GetTexture();
		}
		else if ( m_pTexture )
		{
			m_pTexture->Create();
			m_pTexture->Initialise();
		}
	}

	void TextureUnit::Cleanup()
	{
		if ( m_pTexture )
		{
			m_pTexture->Cleanup();
			m_pTexture->Destroy();
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
			Pipeline & l_pipeline = GetEngine()->GetRenderSystem()->GetCurrentContext()->GetPipeline();
			auto l_return = m_pTexture->Bind( m_index );

			if ( l_return && m_changed && m_bAutoMipmaps && m_pTexture->GetType() != eTEXTURE_TYPE_BUFFER )
			{
				m_pTexture->GenerateMipmaps();
				m_changed = false;
			}

			if ( l_return && GetSampler() )
			{
				l_return = GetSampler()->Bind( m_index );
			}

			l_pipeline.SetTextureMatrix( m_index, m_mtxTransformations );
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

	bool TextureUnit::LoadTexture( Path const & p_pathFile )
	{
		bool l_return = false;

		try
		{
			auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( eTEXTURE_TYPE_2D, eACCESS_TYPE_READ, eACCESS_TYPE_READ );
			l_texture->GetImage().SetSource( p_pathFile );
			SetTexture( l_texture );
			m_pathTexture = p_pathFile;
			m_changed = true;
			l_return = true;
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}

		return l_return;
	}

	bool TextureUnit::IsTextureInitialised()const
	{
		return m_pTexture && m_pTexture->IsInitialised();
	}

	eTEXTURE_TYPE TextureUnit::GetType()const
	{
		REQUIRE( m_pTexture );
		return m_pTexture->GetType();
	}
}
