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
			return ( !source.isRenderTarget() )
				&& !ashes::isCompressedFormat( config.imageInfo->format );
		}

		bool isMergeable( TextureSourceInfo const & source
			, PassTextureConfig const & config
			, AnimationUPtr const & animation )
		{
			return isPreparable( source, config )
				&& !animation;
		}

		uint32_t & getMask( TextureConfiguration & config
			, uint32_t offset )
		{
			return ( *reinterpret_cast< castor::Point2ui * >( reinterpret_cast< uint8_t * >( &config ) + offset ) )[0];
		}

		TextureSourceInfo mergeSourceInfos( TextureSourceInfo const & lhs
			, TextureSourceInfo const & rhs )
		{
			return TextureSourceInfo{ lhs.sampler()
				, lhs.folder()
				, castor::Path{ lhs.relative() + rhs.relative() }
				, lhs.config() };
		}
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent
		, PassTypeID typeID
		, PassFlags initialFlags )
		: OwnedBy< Material >{ parent }
		, m_typeID{ typeID }
		, m_index{ parent.getPassCount() }
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
		, m_renderPassInfo{ getOwner()->getRenderPassInfo() }
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
		vis.visit( cuT( "Transmission" )
			, m_transmission );
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
		doUpdateTextureFlags();
	}

	void Pass::registerTexture( TextureSourceInfo sourceInfo
		, PassTextureConfig configuration
		, AnimationUPtr animation )
	{
		m_animations.emplace( sourceInfo
			, std::move( animation ) );
		registerTexture( std::move( sourceInfo )
			, std::move( configuration ) );
	}

	void Pass::unregisterTexture( TextureSourceInfo sourceInfo )
	{
		auto it = m_sources.find( sourceInfo );

		if ( it != m_sources.end() )
		{
			m_sources.erase( it );
			m_dirty = true;

			remFlag( m_textures, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );
			doUpdateTextureFlags();
		}
	}

	void Pass::resetTexture( TextureSourceInfo const & srcSourceInfo
		, TextureSourceInfo dstSourceInfo )
	{
		auto it = m_sources.find( srcSourceInfo );
		PassTextureConfig configuration;

		if ( it != m_sources.end() )
		{
			configuration = it->second;
			m_sources.erase( it );
			remFlag( m_textures, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );

			it = m_sources.find( dstSourceInfo );

			if ( it != m_sources.end() )
			{
				mergeConfigs( std::move( configuration.config ), it->second.config );
			}
			else
			{
				it = m_sources.emplace( std::move( dstSourceInfo )
					, std::move( configuration ) ).first;
			}

			m_textures |= getFlags( it->second.config );
			doUpdateTextureFlags();
		}
	}

	void Pass::updateConfig( TextureSourceInfo const & sourceInfo
		, TextureConfiguration configuration )
	{
		auto it = m_sources.find( sourceInfo );
		if ( it != m_sources.end() )
		{
			remFlag( m_textures, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );
			it->second.config = std::move( configuration );
			m_textures |= getFlags( it->second.config );
			doUpdateTextureFlags();
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

		doUpdateAlphaFlags();
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
		data.edgeColour->r = powf( getEdgeColour().red(), 2.2f );
		data.edgeColour->g = powf( getEdgeColour().green(), 2.2f );
		data.edgeColour->b = powf( getEdgeColour().blue(), 2.2f );
		data.edgeColour->a = ( checkFlag( m_flags, PassFlag::eDrawEdge )
			? getEdgeColour().alpha()
			: 0.0f );
		data.common->r = 0.0f;
		data.common->g = getEmissive();
		data.common->b = getAlphaValue();
		data.opacity->r = powf( getTransmission()->x, 2.2f );
		data.opacity->g = powf( getTransmission()->y, 2.2f );
		data.opacity->b = powf( getTransmission()->z, 2.2f );
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

			auto lhsAnimIt = m_animations.find( lhsIt.first );
			auto rhsAnimIt = m_animations.find( rhsIt.first );

			auto lhsAnim = ( lhsAnimIt != m_animations.end()
				? std::move( lhsAnimIt->second )
				: nullptr );
			auto rhsAnim = ( rhsAnimIt != m_animations.end()
				? std::move( rhsAnimIt->second )
				: nullptr );

			if ( lhsIt.first == rhsIt.first )
			{
				auto unit = textureCache.getTexture( lhsIt.first, lhsIt.second );

				if ( doAddUnit( lhsIt.second.config
					, std::move( lhsAnim )
					, unit
					, result ) )
				{
					m_unitsSources.emplace( unit.get(), TextureSourceSet{} ).first->second.insert( lhsIt.first );
				}
			}
			else
			{
				auto lhsFlags = getFlags( lhsIt.second.config );
				auto rhsFlags = getFlags( rhsIt.second.config );

				if ( lhsFlags.size() == 1u
					&& rhsFlags.size() == 1u
					&& isMergeable( lhsIt.first, lhsIt.second, lhsAnim )
					&& isMergeable( rhsIt.first, rhsIt.second, rhsAnim ) )
				{
					log::debug << getOwner()->getName() << name << cuT( " - Merging textures." ) << std::endl;
					auto resultSourceInfo = mergeSourceInfos( lhsIt.first, rhsIt.first );
					TextureConfiguration resultConfig;
					resultConfig.heightMask[0] = ( lhsIt.second.config.heightMask[0]
						| rhsIt.second.config.heightMask[0] );
					resultConfig.heightMask[1] = ( lhsIt.second.config.heightMask[1]
						| rhsIt.second.config.heightMask[1] );
					resultConfig.heightFactor = std::min( lhsIt.second.config.heightFactor
						, rhsIt.second.config.heightFactor );
					getMask( resultConfig, lhsMaskOffset ) = lhsDstMask;
					getMask( resultConfig, rhsMaskOffset ) = rhsDstMask;
					auto img = textureCache.mergeImages( lhsIt.first
						, lhsIt.second.config
						, getMask( lhsIt.second.config, lhsMaskOffset )
						, lhsDstMask
						, rhsIt.first
						, rhsIt.second.config
						, getMask( rhsIt.second.config, rhsMaskOffset )
						, rhsDstMask
						, getOwner()->getName() + name
						, resultSourceInfo
						, resultConfig );

					if ( doAddUnit( resultConfig
						, nullptr
						, img
						, result ) )
					{
						m_unitsSources.emplace( img.get(), TextureSourceSet{} ).first->second.insert( lhsIt.first );
						m_unitsSources.emplace( img.get(), TextureSourceSet{} ).first->second.insert( rhsIt.first );
					}
				}
				else
				{
					auto unit = textureCache.getTexture( lhsIt.first, lhsIt.second );

					if ( doAddUnit( lhsIt.second.config
						, std::move( lhsAnim )
						, unit
						, result ) )
					{
						m_unitsSources.emplace( unit.get(), TextureSourceSet{} ).first->second.insert( lhsIt.first );
					}

					unit = textureCache.getTexture( rhsIt.first, rhsIt.second );

					if ( doAddUnit( rhsIt.second.config
						, std::move( rhsAnim )
						, unit
						, result ) )
					{
						m_unitsSources.emplace( unit.get(), TextureSourceSet{} ).first->second.insert( rhsIt.first );
					}
				}
			}
		}
		else
		{
			if ( !sourcesLhs.empty() )
			{
				auto & it = *sourcesLhs.begin();
				auto animIt = m_animations.find( it.first );
				auto anim = ( animIt != m_animations.end()
					? std::move( animIt->second )
					: nullptr );
				auto unit = textureCache.getTexture( it.first, it.second );

				if ( doAddUnit( it.second.config
					, std::move( anim )
					, unit
					, result ) )
				{
					m_unitsSources.emplace( unit.get(), TextureSourceSet{} ).first->second.insert( it.first );
				}
			}
			else if ( !sourcesRhs.empty() )
			{
				auto & it = *sourcesRhs.begin();
				auto animIt = m_animations.find( it.first );
				auto anim = ( animIt != m_animations.end()
					? std::move( animIt->second )
					: nullptr );
				auto unit = textureCache.getTexture( it.first, it.second );

				if ( doAddUnit( it.second.config
					, std::move( anim )
					, unit
					, result ) )
				{
					m_unitsSources.emplace( unit.get(), TextureSourceSet{} ).first->second.insert( it.first );
				}
			}
		}

#ifndef NDEBUG
		auto it = result.begin();

		while ( it != result.end() )
		{
			auto nit = std::find_if( std::next( it )
				, result.end()
				, [it]( TextureUnitSPtr lookup )
				{
					return shallowEqual( ( *it )->getConfiguration(), lookup->getConfiguration() );
				} );
			bool ok = ( nit == result.end() );
			CU_Ensure( ok );
			++it;
		}
#endif
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

	bool Pass::doAddUnit( TextureConfiguration const & config
		, AnimationUPtr animation
		, TextureUnitSPtr unit
		, TextureUnitPtrArray & result )
	{
		if ( config.heightMask[0] )
		{
			m_heightTextureIndex = uint32_t( result.size() );
		}

		if ( animation && !unit->hasAnimation() )
		{
			auto anim = animation.get();
			
			unit->addAnimation( std::move( animation ) );
			static_cast< TextureAnimation & >( *anim ).setAnimable( *unit );
		}

		auto it = std::find_if( result.begin()
			, result.end()
			, [&config]( TextureUnitSPtr lookup )
			{
				return shallowEqual( config, lookup->getConfiguration() );
			} );

		if ( it == result.end() )
		{
			result.push_back( unit );
			return true;
		}

		return false;
	}

	void Pass::doUpdateAlphaFlags()
	{
		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() );
		updateFlag( PassFlag::eBlendAlphaTest, hasBlendAlphaTest() );
	}

	void Pass::doUpdateTextureFlags()
	{
		doUpdateAlphaFlags();

		if ( m_texturesReduced.exchange( false ) )
		{
			prepareTextures();
			onChanged( *this );
		}

		m_dirty = true;
	}
}
