#include "Pass.hpp"

#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		String GetName( TextureChannel p_channel )
		{
			StringStream result;

			switch ( p_channel )
			{
			case TextureChannel::eDiffuse:
				result << cuT( "Diffuse/Albedo" );
				break;

			case TextureChannel::eNormal:
				result << cuT( "Normal" );
				break;

			case TextureChannel::eOpacity:
				result << cuT( "Opacity" );
				break;

			case TextureChannel::eSpecular:
				result << cuT( "Specular/Roughness" );
				break;

			case TextureChannel::eHeight:
				result << cuT( "Height" );
				break;

			case TextureChannel::eGloss:
				result << cuT( "Gloss/Metallic" );
				break;

			case TextureChannel::eEmissive:
				result << cuT( "Emissive" );
				break;

			case TextureChannel::eReflection:
				result << cuT( "Reflection" );
				break;

			case TextureChannel::eRefraction:
				result << cuT( "Refraction" );
				break;

			case TextureChannel::eAmbientOcclusion:
				result << cuT( "Ambient Occlusion" );
				break;

			default:
				break;
			}

			return result.str();
		}
	}

	//*********************************************************************************************
	
	Pass::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Pass >{ p_tabs }
	{
	}

	bool Pass::TextWriter::operator()( Pass const & p_pass, TextFile & p_file )
	{
		static std::map< ComparisonFunc, String > strAlphaFuncs
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
		static const String StrBlendModes[uint32_t( BlendMode::eCount )] =
		{
			cuT( "none" ),
			cuT( "additive" ),
			cuT( "multiplicative" ),
			cuT( "interpolative" ),
			cuT( "a_buffer" ),
			cuT( "depth_peeling" ),
		};

		bool result = true;

		if ( result && p_pass.GetOpacity() < 1 )
		{
			result = p_file.WriteText( m_tabs + cuT( "\talpha " ) + string::to_string( p_pass.GetOpacity() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass opacity" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\temissive " ) + string::to_string( p_pass.GetEmissive() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass emissive" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\ttwo_sided " ) + String( p_pass.IsTwoSided() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass sidedness" );
		}

		if ( result && ( p_pass.GetColourBlendMode() != BlendMode::eInterpolative ) )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tcolour_blend_mode " ) + StrBlendModes[uint32_t( p_pass.GetColourBlendMode() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass colour blend mode" );
		}

		if ( result && ( p_pass.GetAlphaBlendMode() != BlendMode::eInterpolative ) )
		{
			result = p_file.WriteText( m_tabs + cuT( "\talpha_blend_mode " ) + StrBlendModes[uint32_t( p_pass.GetAlphaBlendMode() )] + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass alpha blend mode" );
		}

		if ( result && p_pass.GetAlphaFunc() != ComparisonFunc::eAlways )
		{
			result = p_file.WriteText( m_tabs + cuT( "\talpha_func " )
				+ strAlphaFuncs[p_pass.GetAlphaFunc()] + cuT( " " )
				+ string::to_string( p_pass.GetAlphaValue() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Pass >::CheckError( result, "Pass alpha function" );
		}

		if ( result )
		{
			for ( auto unit : p_pass )
			{
				result = TextureUnit::TextWriter( m_tabs + cuT( "\t" ) )( *unit, p_file );
			}
		}

		return result;
	}

	//*********************************************************************************************

	Pass::Pass( Material & p_parent )
		: OwnedBy< Material >{ p_parent }
	{
	}

	Pass::~Pass()
	{
		m_textureUnits.clear();
	}

	void Pass::Initialise()
	{
		PrepareTextures();

		for ( auto unit : m_textureUnits )
		{
			unit->Initialise();
		}

		DoInitialise();
	}

	void Pass::Cleanup()
	{
		DoCleanup();

		for ( auto unit : m_textureUnits )
		{
			unit->Cleanup();
		}
	}

	void Pass::Update( PassBuffer & p_passes )const
	{
		DoUpdate( p_passes );
	}

	void Pass::BindTextures()
	{
		for ( auto it : m_textureUnits )
		{
			it->Bind();
		}
	}

	void Pass::UnbindTextures()
	{
		for ( auto it : m_textureUnits )
		{
			it->Unbind();
		}
	}

	void Pass::AddTextureUnit( TextureUnitSPtr p_unit )
	{
		m_textureUnits.push_back( p_unit );
		AddFlag( m_textureFlags, p_unit->GetChannel() );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( TextureChannel p_channel )const
	{
		auto it = std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, [p_channel]( TextureUnitSPtr p_unit )
			{
				return p_unit->GetChannel() == p_channel;
			} );

		TextureUnitSPtr result;

		if ( it != m_textureUnits.end() )
		{
			result = *it;
		}

		return result;
	}

	void Pass::DestroyTextureUnit( uint32_t p_index )
	{
		REQUIRE( p_index < m_textureUnits.size() );
		Logger::LogInfo( StringStream() << cuT( "Destroying TextureUnit " ) << p_index );
		auto it = m_textureUnits.begin();
		m_textureUnits.erase( it + p_index );
		uint32_t i = 0u;

		for ( it = m_textureUnits.begin(); it != m_textureUnits.end(); ++it )
		{
			( *it )->SetIndex( ++i );
		}

		DoUpdateFlags();
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::GetTextureUnit( uint32_t p_index )const
	{
		REQUIRE( p_index < m_textureUnits.size() );
		return m_textureUnits[p_index];
	}

	bool Pass::HasAlphaBlending()const
	{
		return ( CheckFlag( m_textureFlags, TextureChannel::eOpacity ) && GetAlphaFunc() == ComparisonFunc::eAlways )
			|| m_opacity < 1.0f;
	}

	void Pass::PrepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto unit : m_textureUnits )
			{
				unit->SetIndex( 0u );
			}

			uint32_t index = MinTextureIndex;
			TextureUnitSPtr opacitySource;
			PxBufferBaseSPtr opacityImage;

			DoPrepareTexture( TextureChannel::eDiffuse, index, opacitySource, opacityImage );
			DoPrepareTexture( TextureChannel::eSpecular, index, opacitySource, opacityImage );
			DoPrepareTexture( TextureChannel::eEmissive, index, opacitySource, opacityImage );

			DoPrepareTexture( TextureChannel::eNormal, index );
			DoPrepareTexture( TextureChannel::eGloss, index );
			DoPrepareTexture( TextureChannel::eHeight, index );

			DoPrepareTexture( TextureChannel::eAmbientOcclusion, index );

			DoPrepareOpacity( opacitySource, opacityImage, index );

			auto unit = GetTextureUnit( TextureChannel::eDiffuse );

			if ( unit )
			{
				if ( unit->GetRenderTarget() )
				{
					m_needsGammaCorrection = false;
				}
				else
				{
					auto format = unit->GetTexture()->GetPixelFormat();
					m_needsGammaCorrection = format != PixelFormat::eL16F32F
						&& format != PixelFormat::eL32F
						&& format != PixelFormat::eAL16F32F
						&& format != PixelFormat::eAL32F
						&& format != PixelFormat::eRGB16F
						&& format != PixelFormat::eRGB16F32F
						&& format != PixelFormat::eRGB32F
						&& format != PixelFormat::eRGBA16F
						&& format != PixelFormat::eRGBA16F32F
						&& format != PixelFormat::eRGBA32F;
				}
			}

			for ( auto unit : m_textureUnits )
			{
				if ( !unit->GetIndex() )
				{
					unit->SetIndex( index++ );
				}
			}

			m_texturesReduced = true;
		}
	}

	MaterialType Pass::GetType()const
	{
		return GetOwner()->GetType();
	}

	void Pass::SetOpacity( float p_value )
	{
		m_opacity = p_value;

		if ( m_opacity < 1.0f && m_alphaBlendMode == BlendMode::eNoBlend )
		{
			m_alphaBlendMode = BlendMode::eInterpolative;
		}

		DoSetOpacity( p_value );
		onChanged( *this );
	}

	ProgramFlags Pass::GetProgramFlags()const
	{
		ProgramFlags result;

		if ( HasAlphaBlending()
			&& !CheckFlag( GetTextureFlags(), TextureChannel::eRefraction ) )
		{
			AddFlag( result, ProgramFlag::eAlphaBlending );
		}

		if ( GetAlphaFunc() != ComparisonFunc::eAlways )
		{
			AddFlag( result, ProgramFlag::eAlphaTest );
		}

		return result;
	}

	bool Pass::DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index, TextureUnitSPtr & p_opacitySource, PxBufferBaseSPtr & p_opacity )
	{
		PxBufferBaseSPtr opacity = DoPrepareTexture( p_channel, p_index );
		bool result = opacity != nullptr;

		if ( result && !p_opacity )
		{
			p_opacity = opacity;
			p_opacitySource = GetTextureUnit( p_channel );
		}

		return result;
	}

	PxBufferBaseSPtr Pass::DoPrepareTexture( TextureChannel p_channel, uint32_t & p_index )
	{
		PxBufferBaseSPtr result;
		auto unit = GetTextureUnit( p_channel );

		if ( unit )
		{
			auto texture = unit->GetTexture();

			if ( texture && texture->GetImage().GetBuffer() )
			{
				PxBufferBaseSPtr extracted = texture->GetImage().GetBuffer();
				result = PF::ExtractAlpha( extracted );

				if ( result )
				{
					texture->SetSource( extracted );
				}
			}

			unit->SetIndex( p_index++ );
			Logger::LogDebug( cuT( "	" ) + GetName( p_channel ) + cuT( " map at index " ) + string::to_string( unit->GetIndex() ) );
		}

		return result;
	}

	void Pass::DoPrepareOpacity( TextureUnitSPtr p_opacitySource, PxBufferBaseSPtr p_opacityImage, uint32_t & p_index )
	{
		TextureUnitSPtr opacityMap = GetTextureUnit( TextureChannel::eOpacity );

		if ( opacityMap
			&& opacityMap->GetTexture()
			&& opacityMap->GetTexture()->GetImage().GetBuffer() )
		{
			PxBufferBaseSPtr reduced = opacityMap->GetTexture()->GetImage().GetBuffer();
			PF::ReduceToAlpha( reduced );
			auto texture = GetOwner()->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			texture->SetSource( reduced );
			opacityMap->SetTexture( texture );
			p_opacityImage.reset();
		}
		else if ( p_opacityImage )
		{
			auto texture = GetOwner()->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			texture->SetSource(p_opacityImage);
			opacityMap = std::make_shared< TextureUnit >( *GetOwner()->GetEngine() );
			opacityMap->SetAutoMipmaps( p_opacitySource->GetAutoMipmaps() );
			opacityMap->SetChannel( TextureChannel::eOpacity );
			opacityMap->SetSampler( p_opacitySource->GetSampler() );
			opacityMap->SetTexture( texture );
			AddTextureUnit( opacityMap );
			p_opacityImage.reset();
		}
		else if ( opacityMap )
		{
			DestroyTextureUnit( opacityMap->GetIndex() );
			opacityMap.reset();
		}

		if ( opacityMap )
		{
			opacityMap->SetIndex( p_index++ );
			Logger::LogDebug( StringStream() << cuT( "	Opacity map at index " ) << opacityMap->GetIndex() );
			AddFlag( m_textureFlags, TextureChannel::eOpacity );

			if ( m_alphaBlendMode == BlendMode::eNoBlend )
			{
				m_alphaBlendMode = BlendMode::eInterpolative;
			}
		}
		else
		{
			RemFlag( m_textureFlags, TextureChannel::eOpacity );
		}
	}

	void Pass::DoUpdateFlags()
	{
		m_textureFlags = 0u;

		for ( auto unit : m_textureUnits )
		{
			if ( unit->GetChannel() != TextureChannel( 0 ) )
			{
				AddFlag( m_textureFlags, unit->GetChannel() );
			}
		}
	}
}
