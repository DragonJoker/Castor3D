#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/PassRenderNode.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		TextureSourceMap getSources( TextureSourceMap const & sources
			, TextureFlags mask )
		{
			TextureSourceMap result;

			for ( auto & source : sources )
			{
				auto & config = source.second.config;

				if ( ( checkFlag( mask, TextureFlag::eAlbedo ) && config.colourMask[0] )
					|| ( checkFlag( mask, TextureFlag::eSpecular ) && config.specularMask[0] )
					|| ( checkFlag( mask, TextureFlag::eMetalness ) && config.metalnessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eGlossiness ) && config.glossinessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eRoughness ) && config.roughnessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eOpacity ) && config.opacityMask[0] )
					|| ( checkFlag( mask, TextureFlag::eEmissive ) && config.emissiveMask[0] )
					|| ( checkFlag( mask, TextureFlag::eNormal ) && config.normalMask[0] )
					|| ( checkFlag( mask, TextureFlag::eHeight ) && config.heightMask[0] )
					|| ( checkFlag( mask, TextureFlag::eOcclusion ) && config.occlusionMask[0] )
					|| ( checkFlag( mask, TextureFlag::eTransmittance ) && config.transmittanceMask[0] ) )
				{
					result.insert( source );
				}
			}

			return result;
		}

		TextureUnitPtrArray getTextureUnits( TextureUnitPtrArray const & units
			, TextureFlags mask )
		{
			TextureUnitPtrArray result;

			for ( auto & unit : units )
			{
				auto & config = unit->getConfiguration();

				if ( ( checkFlag( mask, TextureFlag::eAlbedo ) && config.colourMask[0] )
					|| ( checkFlag( mask, TextureFlag::eSpecular ) && config.specularMask[0] )
					|| ( checkFlag( mask, TextureFlag::eMetalness ) && config.metalnessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eGlossiness ) && config.glossinessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eRoughness ) && config.roughnessMask[0] )
					|| ( checkFlag( mask, TextureFlag::eOpacity ) && config.opacityMask[0] )
					|| ( checkFlag( mask, TextureFlag::eEmissive ) && config.emissiveMask[0] )
					|| ( checkFlag( mask, TextureFlag::eNormal ) && config.normalMask[0] )
					|| ( checkFlag( mask, TextureFlag::eHeight ) && config.heightMask[0] )
					|| ( checkFlag( mask, TextureFlag::eOcclusion ) && config.occlusionMask[0] )
					|| ( checkFlag( mask, TextureFlag::eTransmittance ) && config.transmittanceMask[0] ) )
				{
					result.push_back( unit );
				}
			}

			return result;
		}

		bool isPreparable( TextureSourceInfo const & source
			, PassTextureConfig const & config )
		{
			return ( !source.renderTarget() )
				&& !ashes::isCompressedFormat( config.imageInfo->format );
		}

		bool isMergeable( TextureSourceInfo const & source
			, PassTextureConfig const & config )
		{
			return isPreparable( source, config )
				&& config.transform == TextureTransform{};
		}

		uint32_t & getMask( TextureConfiguration & config
			, uint32_t offset )
		{
			return ( *reinterpret_cast< castor::Point2ui * >( reinterpret_cast< uint8_t * >( &config ) + offset ) )[0];
		}

		void addResult( TextureConfiguration const & config
			, TextureUnitSPtr unit
			, uint32_t & heightTextureIndex
			, TextureUnitPtrArray & result )
		{
			if ( config.heightMask[0] )
			{
				heightTextureIndex = uint32_t( result.size() );
			}

			result.push_back( unit );
		}
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: OwnedBy< Material >{ parent }
		, m_typeID{ typeID }
		, m_flags{ PassFlag::ePickable
			| PassFlag::eLighting
			| initialFlags }
		, m_opacity{ m_dirty, 1.0f }
		, m_bwAccumulationOperator{ m_dirty, castor::makeRangedValue( 1u, 0u, 8u ) }
		, m_emissive{ m_dirty, 0.0f }
		, m_refractionRatio{ m_dirty, 0.0f }
		, m_twoSided{ m_dirty, false }
		, m_alphaBlendMode{ m_dirty, BlendMode::eNoBlend }
		, m_colourBlendMode{ m_dirty, BlendMode::eNoBlend }
		, m_alphaValue{ m_dirty, 0.0f }
		, m_transmission{ m_dirty, { 1.0f, 1.0f, 1.0f } }
		, m_alphaFunc{ m_dirty, VK_COMPARE_OP_ALWAYS }
		, m_blendAlphaFunc{ m_dirty, VK_COMPARE_OP_ALWAYS }
		, m_parallaxOcclusionMode{ m_dirty, ParallaxOcclusionMode::eNone }
		, m_edgeColour{ m_dirty, castor::RgbaColour::fromComponents( 0.0f, 0.0f, 0.0f, 1.0f ) }
		, m_edgeWidth{ m_dirty, { 1.0f, castor::makeRange( MinEdgeWidth, MaxEdgeWidth ) } }
		, m_depthFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_normalFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
		, m_objectFactor{ m_dirty, { 1.0f, castor::makeRange( 0.0f, 1.0f ) } }
	{
	}

	Pass::~Pass()
	{
		CU_Assert( getId() == 0u, "Did you forget to call Pass::cleanup ?" );
	}

	void Pass::initialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		for ( auto & unit : m_textureUnits )
		{
			while ( !unit->isInitialised() )
			{
				std::this_thread::sleep_for( 1_ms );
			}
		}

		getOwner()->getOwner()->getMaterialCache().registerPass( *this );
	}

	void Pass::cleanup()
	{
		getOwner()->getOwner()->getMaterialCache().unregisterPass( *this );
	}

	void Pass::update()
	{
		if ( m_dirty )
		{
			onChanged( *this );
			m_dirty = false;
		}
	}

	void Pass::accept( PassVisitorBase & vis )
	{
		doAccept( vis );
		vis.visit( cuT( "Emissive" )
			, m_emissive );
		vis.visit( cuT( "Opacity" )
			, m_opacity );
		vis.visit( cuT( "Two sided" )
			, m_twoSided );
		vis.visit( cuT( "Colour blend mode" )
			, m_colourBlendMode );
		vis.visit( cuT( "Alpha blend mode" )
			, m_alphaBlendMode );
		vis.visit( cuT( "Pass flags" )
			, m_flags );
		vis.visit( cuT( "Alpha func" )
			, m_alphaFunc );
		vis.visit( cuT( "Blend alpha func" )
			, m_blendAlphaFunc );
		vis.visit( cuT( "Alpha ref. value" )
			, m_alphaValue );
		vis.visit( cuT( "Transmission" )
			, m_transmission );
		vis.visit( cuT( "Blended weighted accumulator" )
			, m_bwAccumulationOperator );
		vis.visit( cuT( "Parallax occlusion mode" )
			, m_parallaxOcclusionMode );

		if ( hasRefraction() )
		{
			vis.visit( cuT( "Refraction ratio" )
				, m_refractionRatio );
		}

		vis.visit( cuT( "Edge colour" )
			, m_edgeColour );
		vis.visit( cuT( "Edge width" )
			, m_edgeWidth );
		vis.visit( cuT( "Depth factor" )
			, m_depthFactor );
		vis.visit( cuT( "Normal factor" )
			, m_normalFactor );
		vis.visit( cuT( "Object factor" )
			, m_objectFactor );
	}

	void Pass::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		doAccept( configuration, vis );
		vis.visit( cuT( "Emissive" ), TextureFlag::eEmissive, configuration.emissiveMask, 3u );
		vis.visit( cuT( "Opacity" ), TextureFlag::eOpacity, configuration.opacityMask, 1u );
		vis.visit( cuT( "Occlusion" ), TextureFlag::eOcclusion, configuration.occlusionMask, 1u );
		vis.visit( cuT( "Transmittance" ), TextureFlag::eTransmittance, configuration.transmittanceMask, 1u );
		vis.visit( cuT( "Normal" ), TextureFlag::eNormal, configuration.normalMask, 3u );
		vis.visit( cuT( "Normal factor" ), configuration.normalFactor );
		vis.visit( cuT( "Normal DirectX" ), configuration.normalGMultiplier );
		vis.visit( cuT( "Height" ), TextureFlag::eHeight, configuration.heightMask, 1u );
		vis.visit( cuT( "Height factor" ), configuration.heightFactor );
	}

	void Pass::registerTexture( TextureSourceInfo sourceInfo
		, PassTextureConfig configuration )
	{
		auto it = m_sources.find( sourceInfo );

		if ( it != m_sources.end() )
		{
			mergeConfigs( std::move( configuration.config ), it->second.config );
		}
		else
		{
			it = m_sources.emplace( std::move( sourceInfo )
				, std::move( configuration ) ).first;
		}

		m_textures |= getFlags( it->second.config );
		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );

		if ( m_texturesReduced.exchange( false ) )
		{
			prepareTextures();
		}

		m_dirty = true;
	}

	void Pass::unregisterTexture( TextureSourceInfo sourceInfo )
	{
		auto it = m_sources.find( sourceInfo );

		if ( it != m_sources.end() )
		{
			m_sources.erase( it );
			m_dirty = true;

			remFlag( m_textures, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );
			updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
			updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
			updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );

			if ( m_texturesReduced.exchange( false ) )
			{
				prepareTextures();
			}
		}
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	bool Pass::needsAlphaProcessing()const
	{
		return checkFlag( m_textures, TextureFlag::eOpacity ) || m_opacity < 1.0f;
	}

	bool Pass::hasAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend;
	}

	bool Pass::hasOnlyAlphaBlending()const
	{
		return needsAlphaProcessing()
			&& getAlphaBlendMode() != BlendMode::eNoBlend
			&& getAlphaFunc() == VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	bool Pass::hasBlendAlphaTest()const
	{
		return needsAlphaProcessing()
			&& getBlendAlphaFunc() != VK_COMPARE_OP_ALWAYS;
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced.exchange( true ) )
		{
			TextureUnitPtrArray newUnits;
			doJoinNmlHgt( newUnits );
			doJoinEmsOcc( newUnits );
			doPrepareTextures( newUnits );
			m_textureUnits = newUnits;
		}
	}

	void Pass::setOpacity( float value )
	{
		m_opacity = value;

		if ( needsAlphaProcessing() )
		{
			if ( m_alphaBlendMode == BlendMode::eNoBlend )
			{
				m_alphaBlendMode = BlendMode::eInterpolative;
			}
		}
		else
		{
			m_alphaBlendMode = BlendMode::eNoBlend;
		}

		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
		m_dirty = true;
	}

	PassFlags Pass::getPassFlags()const
	{
		return m_flags;
	}

	void Pass::setSubsurfaceScattering( SubsurfaceScatteringUPtr value )
	{
		m_subsurfaceScattering = std::move( value );
		updateFlag( PassFlag::eSubsurfaceScattering, m_subsurfaceScattering != nullptr );
		m_sssConnection = m_subsurfaceScattering->onChanged.connect( [this]( SubsurfaceScattering const & sss )
			{
				onSssChanged( sss );
			} );
		m_dirty = true;
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

	TextureUnitPtrArray Pass::getTextureUnits( TextureFlags mask )const
	{
		return castor3d::getTextureUnits( m_textureUnits, mask );
	}

	TextureFlagsArray Pass::getTexturesMask( TextureFlags mask )const
	{
		if ( !m_texturesReduced )
		{
			return {};
		}

		TextureUnitPtrArray units{ getTextureUnits( mask ) };
		TextureFlagsArray result;

		for ( auto & unit : units )
		{
			result.push_back( { unit->getFlags(), unit->getId() } );
		}

		return result;
	}

	uint32_t Pass::getTextureUnitsCount( TextureFlags mask )const
	{
		return uint32_t( castor3d::getTextureUnits( m_textureUnits, mask ).size() );
	}

	void Pass::doFillData( PassBuffer::PassDataPtr & data )const
	{
		data.edgeFactors->r = getEdgeWidth();
		data.edgeFactors->g = getDepthFactor();
		data.edgeFactors->b = getNormalFactor();
		data.edgeFactors->a = getObjectFactor();
		data.edgeColour->r = getEdgeColour().red();
		data.edgeColour->g = getEdgeColour().green();
		data.edgeColour->b = getEdgeColour().blue();
		data.edgeColour->a = ( checkFlag( m_flags, PassFlag::eDrawEdge )
			? getEdgeColour().alpha()
			: 0.0f );
		data.common->r = 0.0f;
		data.common->g = getEmissive();
		data.common->b = getAlphaValue();
		data.common->a = needsGammaCorrection() ? 2.2f : 1.0f;
		data.opacity->r = getTransmission()->x;
		data.opacity->g = getTransmission()->y;
		data.opacity->b = getTransmission()->z;
		data.opacity->a = getOpacity();
		data.reflRefr->r = getRefractionRatio();
		data.reflRefr->g = hasRefraction() ? 1.0f : 0.0f;
		data.reflRefr->b = hasReflections() ? 1.0f : 0.0f;
		data.reflRefr->a = float( getBWAccumulationOperator() );

		if ( hasSubsurfaceScattering() )
		{
			auto & subsurfaceScattering = getSubsurfaceScattering();
			data.extended.sssInfo->r = 1.0f;
			data.extended.sssInfo->g = subsurfaceScattering.getGaussianWidth();
			data.extended.sssInfo->b = subsurfaceScattering.getStrength();
			data.extended.sssInfo->a = float( subsurfaceScattering.getProfileSize() );

			auto i = 0u;
			auto & transmittanceProfile = *data.extended.transmittanceProfile;

			for ( auto & factor : subsurfaceScattering )
			{
				transmittanceProfile[i].r = factor[0];
				transmittanceProfile[i].g = factor[1];
				transmittanceProfile[i].b = factor[2];
				transmittanceProfile[i].a = factor[3];
				++i;
			}
		}
		else
		{
			data.extended.sssInfo->r = 0.0f;
			data.extended.sssInfo->g = 0.0f;
			data.extended.sssInfo->b = 0.0f;
			data.extended.sssInfo->a = 0.0f;
		}
	}

	void Pass::doMergeImages( TextureFlag lhsFlag
		, uint32_t lhsMaskOffset
		, uint32_t lhsDstMask
		, TextureFlag rhsFlag
		, uint32_t rhsMaskOffset
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureUnitPtrArray & result )
	{
		auto & textureCache = getOwner()->getEngine()->getTextureUnitCache();
		auto sourcesLhs = castor3d::getSources( m_sources, lhsFlag );
		auto sourcesRhs = castor3d::getSources( m_sources, rhsFlag );

		if ( !sourcesLhs.empty()
			&& !sourcesRhs.empty() )
		{
			auto & lhsIt = *sourcesLhs.begin();
			auto & rhsIt = *sourcesRhs.begin();

			if ( lhsIt.first == rhsIt.first )
			{
				addResult( lhsIt.second.config
					, textureCache.getTexture( lhsIt.first, lhsIt.second )
					, m_heightTextureIndex
					, result );
			}
			else
			{
				if ( isMergeable( lhsIt.first, lhsIt.second )
					&& isMergeable( rhsIt.first, rhsIt.second ) )
				{
					log::debug << getOwner()->getName() << name << cuT( " - Merging textures." ) << std::endl;
					TextureConfiguration resultConfig;
					resultConfig.heightMask[0] = ( lhsIt.second.config.heightMask[0]
						| rhsIt.second.config.heightMask[0] );
					resultConfig.heightMask[1] = ( lhsIt.second.config.heightMask[1]
						| rhsIt.second.config.heightMask[1] );
					resultConfig.heightFactor = std::min( lhsIt.second.config.heightFactor
						, rhsIt.second.config.heightFactor );
					getMask( resultConfig, lhsMaskOffset ) = lhsDstMask;
					getMask( resultConfig, rhsMaskOffset ) = rhsDstMask;
					addResult( resultConfig
						, textureCache.mergeImages( lhsIt.first
							, lhsIt.second.config
							, getMask( lhsIt.second.config, lhsMaskOffset )
							, lhsDstMask
							, rhsIt.first
							, rhsIt.second.config
							, getMask( rhsIt.second.config, rhsMaskOffset )
							, rhsDstMask
							, getOwner()->getName() + name
							, resultConfig )
						, m_heightTextureIndex
						, result );
				}
				else
				{
					addResult( lhsIt.second.config
						, textureCache.getTexture( lhsIt.first, lhsIt.second )
						, m_heightTextureIndex
						, result );
					addResult( rhsIt.second.config
						, textureCache.getTexture( rhsIt.first, rhsIt.second )
						, m_heightTextureIndex
						, result );
				}
			}
		}
		else
		{
			TextureUnitSPtr unit;

			if ( !sourcesLhs.empty() )
			{
				auto & it = *sourcesLhs.begin();
				addResult( it.second.config
					, textureCache.getTexture( it.first, it.second )
					, m_heightTextureIndex
					, result );
			}
			else if ( !sourcesRhs.empty() )
			{
				auto & it = *sourcesRhs.begin();
				addResult( it.second.config
					, textureCache.getTexture( it.first, it.second )
					, m_heightTextureIndex
					, result );
			}
		}
	}

	void Pass::doJoinDifOpa( TextureUnitPtrArray & result
		, castor::String const & name )
	{
		doMergeImages( TextureFlag::eDiffuse
			, offsetof( TextureConfiguration, colourMask )
			, 0x00FFFFFF
			, TextureFlag::eOpacity
			, offsetof( TextureConfiguration, opacityMask )
			, 0xFF000000
			, name
			, result );
	}

	void Pass::onSssChanged( SubsurfaceScattering const & sss )
	{
		m_dirty = true;
	}

	void Pass::doJoinNmlHgt( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eNormal
			, offsetof( TextureConfiguration, normalMask )
			, 0x00FFFFFF
			, TextureFlag::eHeight
			, offsetof( TextureConfiguration, heightMask )
			, 0xFF000000
			, cuT( "NmlHgt" )
			, result );
	}

	void Pass::doJoinEmsOcc( TextureUnitPtrArray & result )
	{
		doMergeImages( TextureFlag::eEmissive
			, offsetof( TextureConfiguration, emissiveMask )
			, 0x00FFFFFF
			, TextureFlag::eOcclusion
			, offsetof( TextureConfiguration, occlusionMask )
			, 0xFF000000
			, cuT( "EmsOcc" )
			, result );
	}
}
