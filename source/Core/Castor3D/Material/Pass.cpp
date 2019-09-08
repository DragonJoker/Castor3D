#include "Castor3D/Material/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Render/RenderNode/PassRenderNode.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		void mergeMasks( uint32_t toMerge
			, TextureFlag flag
			, TextureFlags & result )
		{
			result |= toMerge
				? flag
				: TextureFlag::eNone;
		}

		TextureFlags getUsedImageComponents( TextureConfiguration const & config )
		{
			TextureFlags result = TextureFlag::eNone;
			mergeMasks( config.colourMask[0], TextureFlag::eDiffuse, result );
			mergeMasks( config.specularMask[0], TextureFlag::eSpecular, result );
			mergeMasks( config.glossinessMask[0], TextureFlag::eGlossiness, result );
			mergeMasks( config.opacityMask[0], TextureFlag::eOpacity, result );
			mergeMasks( config.emissiveMask[0], TextureFlag::eEmissive, result );
			mergeMasks( config.normalMask[0], TextureFlag::eNormal, result );
			mergeMasks( config.heightMask[0], TextureFlag::eHeight, result );
			mergeMasks( config.occlusionMask[0], TextureFlag::eOcclusion, result );
			mergeMasks( config.transmittanceMask[0], TextureFlag::eTransmittance, result );

			if ( checkFlag( config.environment, TextureConfiguration::ReflectionMask ) )
			{
				result |= TextureFlag::eReflection;
			}

			if ( checkFlag( config.environment, TextureConfiguration::RefractionMask ) )
			{
				result |= TextureFlag::eRefraction;
			}

			return result;
		}

		void mergeMasks( uint32_t lhs
			, uint32_t & rhs )
		{
			rhs |= lhs;
		}

		void mergeFactors( float lhs
			, float & rhs
			, float ref )
		{
			if ( lhs != rhs )
			{
				rhs = ( lhs == ref
					? rhs
					: lhs );
			}
		}

		void mergeConfigs( TextureConfiguration const & lhs
			, TextureConfiguration & rhs )
		{
			mergeMasks( lhs.colourMask[0], rhs.colourMask[0] );
			mergeMasks( lhs.specularMask[0], rhs.specularMask[0] );
			mergeMasks( lhs.glossinessMask[0], rhs.glossinessMask[0] );
			mergeMasks( lhs.opacityMask[0], rhs.opacityMask[0] );
			mergeMasks( lhs.emissiveMask[0], rhs.emissiveMask[0] );
			mergeMasks( lhs.normalMask[0], rhs.normalMask[0] );
			mergeMasks( lhs.heightMask[0], rhs.heightMask[0] );
			mergeMasks( lhs.occlusionMask[0], rhs.occlusionMask[0] );
			mergeMasks( lhs.transmittanceMask[0], rhs.transmittanceMask[0] );
			mergeMasks( lhs.environment, rhs.environment );
			mergeMasks( lhs.needsGammaCorrection, rhs.needsGammaCorrection );
			mergeFactors( lhs.heightFactor, rhs.heightFactor, 0.1f );
			mergeFactors( lhs.normalFactor, rhs.normalFactor, 1.0f );
			mergeFactors( lhs.normalGMultiplier, rhs.normalGMultiplier, 1.0f );
		}
	}

	//*********************************************************************************************
	
	Pass::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Pass >{ tabs }
	{
	}

	bool Pass::TextWriter::operator()( Pass const & pass, TextFile & file )
	{
		static std::map< VkCompareOp, String > strAlphaFuncs
		{
			{ VK_COMPARE_OP_ALWAYS, cuT( "always" ) },
			{ VK_COMPARE_OP_LESS, cuT( "less" ) },
			{ VK_COMPARE_OP_LESS_OR_EQUAL, cuT( "less_or_equal" ) },
			{ VK_COMPARE_OP_EQUAL, cuT( "equal" ) },
			{ VK_COMPARE_OP_NOT_EQUAL, cuT( "not_equal" ) },
			{ VK_COMPARE_OP_GREATER_OR_EQUAL, cuT( "greater_or_equal" ) },
			{ VK_COMPARE_OP_GREATER, cuT( "greater" ) },
			{ VK_COMPARE_OP_NEVER, cuT( "never" ) },
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
			result = file.writeText( m_tabs + cuT( "\talpha " ) + string::toString( pass.getOpacity(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			castor::TextWriter< Pass >::checkError( result, "Pass opacity" );

			if ( result && pass.getBWAccumulationOperator() != 0u )
			{
				result = file.writeText( m_tabs + cuT( "\tbw_accumulation " ) + string::toString( uint32_t( pass.getBWAccumulationOperator() ), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
				castor::TextWriter< Pass >::checkError( result, "Pass opacity" );
			}
		}

		if ( result && pass.getEmissive() > 0 )
		{
			result = file.writeText( m_tabs + cuT( "\temissive " ) + string::toString( pass.getEmissive(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
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
			if ( pass.getAlphaFunc() != VK_COMPARE_OP_ALWAYS )
			{
				result = file.writeText( m_tabs + cuT( "\talpha_func " )
					+ strAlphaFuncs[pass.getAlphaFunc()] + cuT( " " )
					+ string::toString( pass.getAlphaValue(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
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
		if ( unit->getConfiguration().environment )
		{
			auto it = std::find_if( m_textureUnits.begin()
				, m_textureUnits.end()
				, []( TextureUnitSPtr lookup )
				{
					return lookup->getConfiguration().environment != 0u;
				} );

			if ( it == m_textureUnits.end() )
			{
				m_textureUnits.push_back( unit );
			}
			else
			{
				auto lhsConfig = unit->getConfiguration();
				unit = *it;
				auto rhsConfig = unit->getConfiguration();
				mergeConfigs( lhsConfig, rhsConfig );
				unit->setConfiguration( rhsConfig );
			}
		}
		else
		{
			auto image = unit->getTexture()->getDefaultImage().toString();
			auto it = std::find_if( m_textureUnits.begin()
				, m_textureUnits.end()
				, [&image]( TextureUnitSPtr lookup )
				{
					return lookup->getTexture()->getDefaultImage().toString() == image;
				} );

			if ( it == m_textureUnits.end() )
			{
				if ( unit->getConfiguration().heightMask[0] )
				{
					m_heightTextureIndex = uint32_t( m_textureUnits.size() );
				}

				m_textureUnits.push_back( unit );
			}
			else
			{
				auto lhsConfig = unit->getConfiguration();
				unit = *it;
				auto rhsConfig = unit->getConfiguration();
				mergeConfigs( lhsConfig, rhsConfig );
				unit->setConfiguration( rhsConfig );

				if ( unit->getConfiguration().heightMask[0] )
				{
					m_heightTextureIndex = uint32_t( std::distance( m_textureUnits.begin(), it ) );
				}
			}
		}

		m_textures |= getUsedImageComponents( unit->getConfiguration() );
		m_texturesReduced = false;
	}

	void Pass::removeTextureUnit( uint32_t index )
	{
		CU_Require( index < m_textureUnits.size() );
		Logger::logInfo( makeStringStream() << cuT( "Destroying TextureUnit " ) << index );
		auto it = m_textureUnits.begin() + index;
		auto config = ( *it )->getConfiguration();
		m_textureUnits.erase( it );
		remFlag( m_textures, TextureFlag( uint16_t( getUsedImageComponents( config ) ) ) );
		m_texturesReduced = false;
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	bool Pass::hasAlphaBlending()const
	{
		return ( checkFlag( m_textures, TextureFlag::eOpacity ) || m_opacity < 1.0f )
			&& getAlphaFunc() == VK_COMPARE_OP_ALWAYS;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced )
		{
			TextureUnitSPtr opacitySource;
			PxBufferBaseSPtr opacityImage;

			for ( auto & unit : m_textureUnits )
			{
				auto configuration = unit->getConfiguration();

				if ( unit->getRenderTarget() )
				{
					configuration.needsGammaCorrection = 0u;
				}
				else if ( configuration.colourMask[0]
					|| configuration.emissiveMask[0] )
				{
					auto format = unit->getTexture()->getPixelFormat();
					configuration.needsGammaCorrection = !PF::isFloatingPoint( convert( format ) );
				}

				unit->setConfiguration( configuration );
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
			&& m_alphaFunc == VK_COMPARE_OP_ALWAYS )
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
			&& !checkFlag( m_textures, TextureFlag::eRefraction ) )
		{
			result |= PassFlag::eAlphaBlending;
		}

		if ( getAlphaFunc() != VK_COMPARE_OP_ALWAYS )
		{
			result |= PassFlag::eAlphaTest;
		}

		switch ( getType() )
		{
		case MaterialType::eMetallicRoughness:
			result |= PassFlag::eMetallicRoughness;
			break;

		case MaterialType::eSpecularGlossiness:
			result |= PassFlag::eSpecularGlossiness;
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

	bool Pass::needsGammaCorrection()const
	{
		return m_textureUnits.end() != std::find_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, []( TextureUnitSPtr unit )
			{
				return unit->getConfiguration().needsGammaCorrection != 0u;
			} );
	}

	uint32_t Pass::getNonEnvTextureUnitsCount()const
	{
		return uint32_t( std::count_if( m_textureUnits.begin()
			, m_textureUnits.end()
			, []( TextureUnitSPtr unit )
			{
				return unit->getConfiguration().environment == 0u;
			} ) );
	}

	void Pass::onSssChanged( SubsurfaceScattering const & sss )
	{
		onChanged( *this );
	}
}
