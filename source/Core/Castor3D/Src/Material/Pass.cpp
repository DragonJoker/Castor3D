#include "Pass.hpp"

#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		String getName( TextureChannel channel )
		{
			StringStream result;

			switch ( channel )
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

			case TextureChannel::eTransmittance:
				result << cuT( "Transmittance" );
				break;

			default:
				break;
			}

			return result.str();
		}
	}

	//*********************************************************************************************
	
	Pass::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Pass >{ tabs }
	{
	}

	bool Pass::TextWriter::operator()( Pass const & pass, TextFile & file )
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

		if ( result && pass.getOpacity() < 1 )
		{
			result = file.writeText( m_tabs + cuT( "\talpha " ) + string::toString( pass.getOpacity() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Pass >::checkError( result, "Pass opacity" );
		}

		if ( result && pass.getEmissive() > 0 )
		{
			result = file.writeText( m_tabs + cuT( "\temissive " ) + string::toString( pass.getEmissive() ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Pass >::checkError( result, "Pass emissive" );
		}

		if ( result && pass.IsTwoSided() )
		{
			result = file.writeText( m_tabs + cuT( "\ttwo_sided " ) + String( pass.IsTwoSided() ? cuT( "true" ) : cuT( "false" ) ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Pass >::checkError( result, "Pass sidedness" );
		}

		if ( result && ( pass.getColourBlendMode() != BlendMode::eNoBlend ) )
		{
			result = file.writeText( m_tabs + cuT( "\tcolour_blend_mode " ) + StrBlendModes[uint32_t( pass.getColourBlendMode() )] + cuT( "\n" ) ) > 0;
			castor::TextWriter< Pass >::checkError( result, "Pass colour blend mode" );
		}

		if ( result )
		{
			if ( pass.getAlphaFunc() != ComparisonFunc::eAlways )
			{
				result = file.writeText( m_tabs + cuT( "\talpha_func " )
					+ strAlphaFuncs[pass.getAlphaFunc()] + cuT( " " )
					+ string::toString( pass.getAlphaValue() ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Pass >::checkError( result, "Pass alpha function" );
			}
			else if ( pass.hasAlphaBlending()
				&& pass.getAlphaBlendMode() != BlendMode::eNoBlend )
			{
				result = file.writeText( m_tabs + cuT( "\talpha_blend_mode " ) + StrBlendModes[uint32_t( pass.getAlphaBlendMode() )] + cuT( "\n" ) ) > 0;
				castor::TextWriter< Pass >::checkError( result, "Pass alpha blend mode" );
			}
		}

		if ( result && pass.hasParallaxOcclusion() )
		{
			result = file.writeText( m_tabs + cuT( "\tparallax_occlusion true\n" ) ) > 0;
		}

		if ( result )
		{
			for ( auto unit : pass )
			{
				result &= TextureUnit::TextWriter{ m_tabs + cuT( "\t" ), pass.getType() }( *unit, file );
			}
		}

		if ( result && pass.hasSubsurfaceScattering() )
		{
			result = SubsurfaceScattering::TextWriter{ m_tabs + cuT( "\t" ) }( pass.getSubsurfaceScattering(), file );
		}

		return result;
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent )
		: OwnedBy< Material >{ parent }
	{
	}

	Pass::~Pass()
	{
		m_textureUnits.clear();
	}

	void Pass::initialise()
	{
		prepareTextures();

		for ( auto unit : m_textureUnits )
		{
			unit->initialise();
		}

		doInitialise();
	}

	void Pass::cleanup()
	{
		doCleanup();

		for ( auto unit : m_textureUnits )
		{
			unit->cleanup();
		}
	}

	void Pass::bindTextures()
	{
		for ( auto it : m_textureUnits )
		{
			it->bind();
		}
	}

	void Pass::unbindTextures()
	{
		for ( auto it : m_textureUnits )
		{
			it->unbind();
		}
	}

	void Pass::addTextureUnit( TextureUnitSPtr unit )
	{
		m_textureUnits.push_back( unit );
		addFlag( m_textureFlags, unit->getChannel() );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::getTextureUnit( TextureChannel channel )const
	{
		auto it = std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, [channel]( TextureUnitSPtr unit )
			{
				return unit->getChannel() == channel;
			} );

		TextureUnitSPtr result;

		if ( it != m_textureUnits.end() )
		{
			result = *it;
		}

		return result;
	}

	void Pass::destroyTextureUnit( uint32_t index )
	{
		REQUIRE( index < m_textureUnits.size() );
		Logger::logInfo( StringStream() << cuT( "Destroying TextureUnit " ) << index );
		auto it = m_textureUnits.begin();
		m_textureUnits.erase( it + index );
		uint32_t i = MinTextureIndex;

		for ( it = m_textureUnits.begin(); it != m_textureUnits.end(); ++it )
		{
			( *it )->setIndex( ++i );
		}

		doUpdateFlags();
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		REQUIRE( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	bool Pass::hasAlphaBlending()const
	{
		return ( checkFlag( m_textureFlags, TextureChannel::eOpacity ) || m_opacity < 1.0f )
			&& getAlphaFunc() == ComparisonFunc::eAlways;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced )
		{
			for ( auto unit : m_textureUnits )
			{
				unit->setIndex( 0u );
			}

			TextureUnitSPtr opacitySource;
			PxBufferBaseSPtr opacityImage;

			uint32_t index = MinTextureIndex;
			doPrepareTexture( TextureChannel::eDiffuse, index );
			doPrepareTexture( TextureChannel::eSpecular, index );
			doPrepareTexture( TextureChannel::eGloss, index );
			doPrepareTexture( TextureChannel::eNormal, index );
			doPrepareOpacity( opacitySource, opacityImage, index );
			doPrepareTexture( TextureChannel::eHeight, index );
			doPrepareTexture( TextureChannel::eAmbientOcclusion, index );
			doPrepareTexture( TextureChannel::eEmissive, index );
			doPrepareTexture( TextureChannel::eTransmittance, index );

			doReduceTexture( TextureChannel::eSpecular
				, getType() == MaterialType::ePbrMetallicRoughness
					? PixelFormat::eL8
					: PixelFormat::eR8G8B8 );
			doReduceTexture( TextureChannel::eGloss
				, PixelFormat::eL8 );
			doReduceTexture( TextureChannel::eHeight
				, PixelFormat::eL8 );
			doReduceTexture( TextureChannel::eAmbientOcclusion
				, PixelFormat::eL8 );
			doReduceTexture( TextureChannel::eTransmittance
				, PixelFormat::eL8 );

			auto unit = getTextureUnit( TextureChannel::eDiffuse );

			if ( unit )
			{
				if ( unit->getRenderTarget() )
				{
					m_needsGammaCorrection = false;
				}
				else if ( unit->getTexture() )
				{
					auto format = unit->getTexture()->getPixelFormat();
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
				if ( !unit->getIndex() )
				{
					unit->setIndex( index++ );
				}
			}

			m_texturesReduced = true;
		}
	}

	MaterialType Pass::getType()const
	{
		return getOwner()->getType();
	}

	void Pass::setOpacity( float value )
	{
		m_opacity = value;

		if ( m_opacity < 1.0f
			&& m_alphaBlendMode == BlendMode::eNoBlend
			&& m_alphaFunc == ComparisonFunc::eAlways )
		{
			m_alphaBlendMode = BlendMode::eInterpolative;
		}

		doSetOpacity( value );
		onChanged( *this );
	}

	PassFlags Pass::getPassFlags()const
	{
		PassFlags result;

		if ( hasAlphaBlending()
			&& !checkFlag( getTextureFlags(), TextureChannel::eRefraction ) )
		{
			result |= PassFlag::eAlphaBlending;
		}

		if ( getAlphaFunc() != ComparisonFunc::eAlways )
		{
			result |= PassFlag::eAlphaTest;
		}

		switch ( getType() )
		{
		case MaterialType::ePbrMetallicRoughness:
			result |= PassFlag::ePbrMetallicRoughness;
			break;

		case MaterialType::ePbrSpecularGlossiness:
			result |= PassFlag::ePbrSpecularGlossiness;
			break;
		}

		if ( hasSubsurfaceScattering() )
		{
			result |= PassFlag::eSubsurfaceScattering;
		}

		if ( hasParallaxOcclusion() )
		{
			result |= PassFlag::eParallaxOcclusionMapping;
		}

		return result;
	}

	bool Pass::doPrepareTexture( TextureChannel channel
		, uint32_t & index
		, TextureUnitSPtr & opacitySource
		, PxBufferBaseSPtr & opacityBuffer )
	{
		PxBufferBaseSPtr opacity = doPrepareTexture( channel, index );
		bool result = opacity != nullptr;

		if ( result && !opacityBuffer )
		{
			opacityBuffer = opacity;
			opacitySource = getTextureUnit( channel );
		}

		return result;
	}

	PxBufferBaseSPtr Pass::doPrepareTexture( TextureChannel channel
		, uint32_t & index )
	{
		PxBufferBaseSPtr result;
		auto unit = getTextureUnit( channel );

		if ( unit )
		{
			auto texture = unit->getTexture();

			if ( texture && texture->getImage().getBuffer() )
			{
				PxBufferBaseSPtr extracted = texture->getImage().getBuffer();
				result = PF::extractAlpha( extracted );

				if ( result )
				{
					texture->setSource( extracted );
				}
			}

			unit->setIndex( index++ );
			Logger::logDebug( cuT( "	" ) + getName( channel ) + cuT( " map at index " ) + string::toString( unit->getIndex() ) );
		}

		return result;
	}

	void Pass::doPrepareOpacity( TextureUnitSPtr opacitySource
		, PxBufferBaseSPtr opacityBuffer
		, uint32_t & index )
	{
		TextureUnitSPtr opacityMap = getTextureUnit( TextureChannel::eOpacity );

		if ( opacityMap
			&& opacityMap->getTexture()
			&& opacityMap->getTexture()->getImage().getBuffer() )
		{
			if ( opacityMap->getTexture()->getImage().getBuffer()->format() != PixelFormat::eL8
				&& opacityMap->getTexture()->getImage().getBuffer()->format() != PixelFormat::eL16F32F
				&& opacityMap->getTexture()->getImage().getBuffer()->format() != PixelFormat::eL32F )
			{
				PxBufferBaseSPtr reduced = opacityMap->getTexture()->getImage().getBuffer();
				PF::reduceToAlpha( reduced );
				auto texture = getOwner()->getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
				texture->setSource( reduced );
				opacityMap->setTexture( texture );
				opacityBuffer.reset();
			}
		}
		else if ( opacityBuffer )
		{
			auto texture = getOwner()->getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
			texture->setSource( opacityBuffer );
			opacityMap = std::make_shared< TextureUnit >( *getOwner()->getEngine() );
			opacityMap->setAutoMipmaps( opacitySource->getAutoMipmaps() );
			opacityMap->setChannel( TextureChannel::eOpacity );
			opacityMap->setSampler( opacitySource->getSampler() );
			opacityMap->setTexture( texture );
			addTextureUnit( opacityMap );
			opacityBuffer.reset();
		}
		else if ( opacityMap )
		{
			destroyTextureUnit( opacityMap->getIndex() );
			opacityMap.reset();
		}

		if ( opacityMap )
		{
			opacityMap->setIndex( index++ );
			Logger::logDebug( StringStream() << cuT( "	Opacity map at index " ) << opacityMap->getIndex() );
			addFlag( m_textureFlags, TextureChannel::eOpacity );

			if ( m_alphaBlendMode == BlendMode::eNoBlend )
			{
				m_alphaBlendMode = BlendMode::eInterpolative;
			}
		}
		else
		{
			remFlag( m_textureFlags, TextureChannel::eOpacity );
		}
	}

	void Pass::doUpdateFlags()
	{
		m_textureFlags = 0u;

		for ( auto unit : m_textureUnits )
		{
			if ( unit->getChannel() != TextureChannel( 0 ) )
			{
				addFlag( m_textureFlags, unit->getChannel() );
			}
		}
	}

	void Pass::doReduceTexture( TextureChannel channel, PixelFormat format )
	{
		auto unit = getTextureUnit( channel );

		if ( unit )
		{
			auto texture = unit->getTexture();

			if ( texture && texture->getImage().getBuffer() )
			{
				auto buffer = texture->getImage().getBuffer();

				if ( buffer->format() != format )
				{
					buffer = PxBufferBase::create( buffer->dimensions(), format, buffer->constPtr(), buffer->format() );
				}
			}
		}
	}
}
