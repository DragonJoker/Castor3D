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
			StringStream result( makeStringStream() );

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
		static std::map< renderer::CompareOp, String > strAlphaFuncs
		{
			{ renderer::CompareOp::eAlways, cuT( "always" ) },
			{ renderer::CompareOp::eLess, cuT( "less" ) },
			{ renderer::CompareOp::eLessEqual, cuT( "less_or_equal" ) },
			{ renderer::CompareOp::eEqual, cuT( "equal" ) },
			{ renderer::CompareOp::eNotEqual, cuT( "not_equal" ) },
			{ renderer::CompareOp::eGreaterEqual, cuT( "greater_or_equal" ) },
			{ renderer::CompareOp::eGreater, cuT( "greater" ) },
			{ renderer::CompareOp::eNever, cuT( "never" ) },
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
			if ( pass.getAlphaFunc() != renderer::CompareOp::eAlways )
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
		Logger::logInfo( makeStringStream() << cuT( "Destroying TextureUnit " ) << index );
		auto it = m_textureUnits.begin();
		m_textureUnits.erase( it + index );
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
			&& getAlphaFunc() == renderer::CompareOp::eAlways;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced )
		{
			TextureUnitSPtr opacitySource;
			PxBufferBaseSPtr opacityImage;

			uint32_t index = MinBufferIndex;
			doPrepareTexture( TextureChannel::eDiffuse, index );
			doPrepareTexture( TextureChannel::eSpecular, index );
			doPrepareTexture( TextureChannel::eGloss, index );
			doPrepareTexture( TextureChannel::eNormal, index );
			doPrepareOpacity( index );
			doPrepareTexture( TextureChannel::eHeight, index );
			doPrepareTexture( TextureChannel::eAmbientOcclusion, index );
			doPrepareTexture( TextureChannel::eEmissive, index );
			doPrepareTexture( TextureChannel::eTransmittance, index );

			doReduceTexture( TextureChannel::eSpecular
				, getType() == MaterialType::ePbrMetallicRoughness
					? renderer::Format::eR8_UNORM
					: renderer::Format::eR8G8B8A8_UNORM );
			doReduceTexture( TextureChannel::eGloss
				, renderer::Format::eR8_UNORM );
			doReduceTexture( TextureChannel::eHeight
				, renderer::Format::eR8_UNORM );
			doReduceTexture( TextureChannel::eAmbientOcclusion
				, renderer::Format::eR8_UNORM );
			doReduceTexture( TextureChannel::eTransmittance
				, renderer::Format::eR8_UNORM );

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
					m_needsGammaCorrection = format != renderer::Format::eR16_SFLOAT
						&& format != renderer::Format::eR32_SFLOAT
						&& format != renderer::Format::eR16G16_SFLOAT
						&& format != renderer::Format::eR32G32_SFLOAT
						&& format != renderer::Format::eR16G16B16_SFLOAT
						&& format != renderer::Format::eR32G32B32_SFLOAT
						&& format != renderer::Format::eR16G16B16A16_SFLOAT
						&& format != renderer::Format::eR32G32B32A32_SFLOAT;
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
			&& m_alphaFunc == renderer::CompareOp::eAlways )
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

		if ( getAlphaFunc() != renderer::CompareOp::eAlways )
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

	void Pass::setSubsurfaceScattering( SubsurfaceScatteringUPtr && value )
	{
		m_subsurfaceScattering = std::move( value );
		m_sssConnection = m_subsurfaceScattering->onChanged.connect( [this]( SubsurfaceScattering const & sss )
			{
				onSssChanged( sss );
			} );
		onChanged( *this );
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
			Logger::logDebug( cuT( "	" ) + getName( channel ) + cuT( " map at index " ) + string::toString( index ) );
			++index;
		}

		return result;
	}

	void Pass::doPrepareOpacity( uint32_t & index )
	{
		TextureUnitSPtr opacityMap = getTextureUnit( TextureChannel::eOpacity );

		if ( opacityMap
			&& opacityMap->getTexture()
			&& opacityMap->getTexture()->getDefaultImage().getBuffer() )
		{
			auto buffer = opacityMap->getTexture()->getDefaultImage().getBuffer();

			if ( buffer->format() != PixelFormat::eL8
				&& buffer->format() != PixelFormat::eL16F
				&& buffer->format() != PixelFormat::eL32F )
			{
				PxBufferBaseSPtr reduced = buffer;
				PF::reduceToAlpha( reduced );
				auto size = reduced->dimensions();
				renderer::ImageCreateInfo createInfo{};
				createInfo.flags = 0u;
				createInfo.arrayLayers = 1u;
				createInfo.extent.width = size.getWidth();
				createInfo.extent.height = size.getHeight();
				createInfo.extent.depth = 1u;
				createInfo.format = convert( reduced->format() );
				createInfo.imageType = renderer::TextureType::e2D;
				createInfo.initialLayout = renderer::ImageLayout::eUndefined;
				createInfo.mipLevels = 1u;
				createInfo.samples = renderer::SampleCountFlag::e1;
				createInfo.sharingMode = renderer::SharingMode::eExclusive;
				createInfo.tiling = renderer::ImageTiling::eOptimal;
				createInfo.usage = renderer::ImageUsageFlag::eSampled;
				auto texture = std::make_shared< TextureLayout >( *getOwner()->getEngine()->getRenderSystem()
					, createInfo
					, renderer::MemoryPropertyFlag::eDeviceLocal );
				texture->setSource( reduced );
				opacityMap->setTexture( texture );
			}
		}
		else if ( opacityMap )
		{
			auto it = std::find( m_textureUnits.begin()
				, m_textureUnits.end()
				, opacityMap );
			REQUIRE( it != m_textureUnits.end() );
			m_textureUnits.erase( it );
			doUpdateFlags();
			m_texturesReduced = false;
			opacityMap.reset();
		}

		if ( opacityMap )
		{
			Logger::logDebug( makeStringStream() << cuT( "	Opacity map at index " ) << index );
			++index;
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

	void Pass::doReduceTexture( TextureChannel channel, renderer::Format format )
	{
		auto unit = getTextureUnit( channel );

		if ( unit )
		{
			auto texture = unit->getTexture();

			if ( texture && texture->getDefaultImage().getBuffer() )
			{
				auto buffer = texture->getDefaultImage().getBuffer();

				if ( buffer->format() != convert( format ) )
				{
					buffer = PxBufferBase::create( buffer->dimensions(), convert( format ), buffer->constPtr(), buffer->format() );
				}
			}
		}
	}

	void Pass::onSssChanged( SubsurfaceScattering const & sss )
	{
		onChanged( *this );
	}
}
