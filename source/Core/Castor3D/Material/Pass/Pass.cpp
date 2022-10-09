#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/PixelFormat.hpp>

#include <algorithm>

namespace castor3d
{
	//*********************************************************************************************

	namespace matpass
	{
		static bool matchConfigFlags( TextureConfiguration const & config
			, TextureFlags mask )
		{
			return ( checkFlag( mask, TextureFlag::eColour ) && config.colourMask[0] )
				|| ( checkFlag( mask, TextureFlag::eSpecular ) && config.specularMask[0] )
				|| ( checkFlag( mask, TextureFlag::eMetalness ) && config.metalnessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eGlossiness ) && config.glossinessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eRoughness ) && config.roughnessMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOpacity ) && config.opacityMask[0] )
				|| ( checkFlag( mask, TextureFlag::eEmissive ) && config.emissiveMask[0] )
				|| ( checkFlag( mask, TextureFlag::eNormal ) && config.normalMask[0] )
				|| ( checkFlag( mask, TextureFlag::eHeight ) && config.heightMask[0] )
				|| ( checkFlag( mask, TextureFlag::eOcclusion ) && config.occlusionMask[0] )
				|| ( checkFlag( mask, TextureFlag::eTransmittance ) && config.transmittanceMask[0] );
		}

		static TextureSourceMap getSources( TextureSourceMap const & sources
			, TextureFlags mask )
		{
			TextureSourceMap result;

			for ( auto & source : sources )
			{
				auto & config = source.second.config;

				if ( matchConfigFlags( config, mask ) )
				{
					result.insert( source );
				}
			}

			return result;
		}

		static TextureUnitPtrArray getTextureUnits( TextureUnitPtrArray const & units
			, TextureFlags mask )
		{
			TextureUnitPtrArray result;

			for ( auto & unit : units )
			{
				auto & config = unit->getConfiguration();

				if ( matchConfigFlags( config, mask ) )
				{
					result.push_back( unit );
				}
			}

			return result;
		}

		static bool isPreparable( TextureSourceInfo const & source
			, PassTextureConfig const & config )
		{
			return ( !source.isRenderTarget() )
				&& !ashes::isCompressedFormat( config.imageInfo->format );
		}

		static bool isMergeable( TextureSourceInfo const & source
			, PassTextureConfig const & config
			, AnimationUPtr const & animation )
		{
			return isPreparable( source, config )
				&& !animation;
		}

		static bool areFormatsCompatible( VkFormat lhs, VkFormat rhs )
		{
			if ( isSRGBFormat( convert( rhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( lhs ) ) );
			}
			else if ( isSRGBFormat( convert( lhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( rhs ) ) );
			}

			return !ashes::isCompressedFormat( lhs )
				&& !ashes::isCompressedFormat( rhs );
		}

		static uint32_t & getMask( TextureConfiguration & config
			, uint32_t offset )
		{
			return ( *reinterpret_cast< castor::Point2ui * >( reinterpret_cast< uint8_t * >( &config ) + offset ) )[0];
		}

		static TextureSourceInfo mergeSourceInfos( TextureSourceInfo const & lhs
			, TextureSourceInfo const & rhs )
		{
			auto folder = lhs.isFileImage()
				? lhs.folder()
				: ( rhs.isFileImage()
					? rhs.folder()
					: castor::Path{} );
			auto lhsName = lhs.isFileImage()
				? lhs.relative()
				: castor::Path{ lhs.name() }.getFileName();
			auto rhsName = rhs.isFileImage()
				? rhs.relative()
				: castor::Path{ rhs.name() }.getFileName();
			auto name = castor::string::getLongestCommonSubstring( lhsName, rhsName );
			castor::string::replace( lhsName, name, castor::String{} );
			castor::string::replace( rhsName, name, castor::String{} );
			return TextureSourceInfo{ lhs.sampler()
				, folder
				, castor::Path{ name + lhsName + rhsName }
				, lhs.config() };
		}

		static float mix( float x, float y, float a )
		{
			return float( x * ( 1.0 - a ) + y * a );
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
			| initialFlags }
		, m_renderPassInfo{ getOwner()->getRenderPassInfo() }
	{
		createComponent< BlendComponent >();
		createComponent< PassHeaderComponent >();
		createComponent< TwoSidedComponent >();
		createComponent< TexturesComponent >();
		createComponent< TextureCountComponent >();
		createComponent< UntileComponent >();
		createComponent< NormalComponent >();
	}

	Pass::~Pass()
	{
		CU_Assert( getId() == 0u, "Did you forget to call Pass::cleanup ?" );
	}

	void Pass::initialise()
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
		if ( m_dirty.exchange( false ) )
		{
			onChanged( *this );
		}

		for ( auto & component : m_components )
		{
			component.second->update();
		}
	}

	void Pass::addComponent( PassComponentUPtr component )
	{
		auto id = getOwner()->getEngine()->getPassComponentsRegister().getNameId( component->getType() );
		m_components.emplace( id, std::move( component ) );
		m_dirty = true;
	}

	bool Pass::hasComponent( castor::String const & name )const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );
		return it != m_components.end();
	}

	PassComponent * Pass::getComponent( castor::String const & name )const
	{
		PassComponent * result{};
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );

		if ( it != m_components.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	void Pass::removeComponent( castor::String const & name )
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );

		if ( it != m_components.end() )
		{
			m_components.erase( it );
			m_dirty = true;
		}
	}

	TextureUnitSPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
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
			m_maxTexcoordSet = std::max( m_maxTexcoordSet, configuration.texcoordSet );
			it = m_sources.emplace( std::move( sourceInfo )
				, std::move( configuration ) ).first;
		}

		m_textureFlags |= getFlags( it->second.config );
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

			remFlag( m_textureFlags, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );
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
			remFlag( m_textureFlags, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );

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

			m_textureFlags |= getFlags( it->second.config );
			doUpdateTextureFlags();
		}
	}

	void Pass::updateConfig( TextureSourceInfo const & sourceInfo
		, TextureConfiguration configuration )
	{
		auto it = m_sources.find( sourceInfo );

		if ( it != m_sources.end() )
		{
			remFlag( m_textureFlags, TextureFlag( uint16_t( getFlags( it->second.config ) ) ) );
			it->second.config = std::move( configuration );
			m_textureFlags |= getFlags( it->second.config );
			doUpdateTextureFlags();
		}
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced.exchange( true ) )
		{
			TextureUnitDataSet sources;

			for ( auto & component : m_components )
			{
				component.second->mergeImages( sources );
			}

			TextureUnitPtrArray newUnits;

			for ( auto & source : sources )
			{
				auto & textureCache = getOwner()->getEngine()->getTextureUnitCache();
				auto unit = textureCache.getTexture( *source );
				doAddUnit( *source, unit, newUnits );
			}

			m_textureUnits = newUnits;
		}
	}

	void Pass::mergeImages( TextureFlag lhsFlag
		, uint32_t lhsMaskOffset
		, uint32_t lhsDstMask
		, TextureFlag rhsFlag
		, uint32_t rhsMaskOffset
		, uint32_t rhsDstMask
		, castor::String const & name
		, TextureUnitDataSet & result )
	{
		auto & engine = *getOwner()->getEngine();
		auto & textureCache = engine.getTextureUnitCache();
		auto sourcesLhs = matpass::getSources( m_sources, lhsFlag );
		auto sourcesRhs = matpass::getSources( m_sources, rhsFlag );

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
				result.emplace( &textureCache.getSourceData( lhsIt.first, lhsIt.second, std::move( lhsAnim ) ) );
			}
			else
			{
				auto & imgCache = engine.getImageCache();

				if ( ( lhsIt.first.isFileImage() || lhsIt.first.isBufferImage() )
					&& lhsIt.second.imageInfo->format == VK_FORMAT_UNDEFINED )
				{
					lhsIt.second.imageInfo->format = convert( imgCache.getImageFormat( lhsIt.first.name() ) );
				}

				if ( ( rhsIt.first.isFileImage() || rhsIt.first.isBufferImage() )
					&& rhsIt.second.imageInfo->format == VK_FORMAT_UNDEFINED )
				{
					rhsIt.second.imageInfo->format = convert( imgCache.getImageFormat( rhsIt.first.name() ) );
				}

				auto lhsFlags = getFlags( lhsIt.second.config );
				auto rhsFlags = getFlags( rhsIt.second.config );

				if ( lhsFlags.size() == 1u
					&& rhsFlags.size() == 1u
					&& lhsIt.second.texcoordSet == rhsIt.second.texcoordSet
					&& matpass::isMergeable( lhsIt.first, lhsIt.second, lhsAnim )
					&& matpass::isMergeable( rhsIt.first, rhsIt.second, rhsAnim )
					&& matpass::areFormatsCompatible( lhsIt.second.imageInfo->format, rhsIt.second.imageInfo->format ) )
				{
					log::debug << getOwner()->getName() << name << cuT( " - Merging textures." ) << std::endl;
					auto resultSourceInfo = matpass::mergeSourceInfos( lhsIt.first, rhsIt.first );
					PassTextureConfig resultConfig;
					resultConfig.config.heightMask[0] = ( lhsIt.second.config.heightMask[0]
						| rhsIt.second.config.heightMask[0] );
					resultConfig.config.heightMask[1] = ( lhsIt.second.config.heightMask[1]
						| rhsIt.second.config.heightMask[1] );
					resultConfig.config.heightFactor = std::min( lhsIt.second.config.heightFactor
						, rhsIt.second.config.heightFactor );
					matpass::getMask( resultConfig.config, lhsMaskOffset ) = lhsDstMask;
					matpass::getMask( resultConfig.config, rhsMaskOffset ) = rhsDstMask;
					mergeConfigsBase( lhsIt.second.config, resultConfig.config );
					mergeConfigsBase( rhsIt.second.config, resultConfig.config );
					result.emplace( &textureCache.mergeSources( lhsIt.first
						, lhsIt.second
						, matpass::getMask( lhsIt.second.config, lhsMaskOffset )
						, lhsDstMask
						, rhsIt.first
						, rhsIt.second
						, matpass::getMask( rhsIt.second.config, rhsMaskOffset )
						, rhsDstMask
						, getOwner()->getName() + name
						, resultSourceInfo
						, resultConfig ) );
				}
				else
				{
					result.emplace( &textureCache.getSourceData( lhsIt.first, lhsIt.second, std::move( lhsAnim ) ) );
					result.emplace( &textureCache.getSourceData( rhsIt.first, rhsIt.second, std::move( rhsAnim ) ) );
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
				result.emplace( &textureCache.getSourceData( it.first, it.second, std::move( anim ) ) );
			}
			else if ( !sourcesRhs.empty() )
			{
				auto & it = *sourcesRhs.begin();
				auto animIt = m_animations.find( it.first );
				auto anim = ( animIt != m_animations.end()
					? std::move( animIt->second )
					: nullptr );
				result.emplace( &textureCache.getSourceData( it.first, it.second, std::move( anim ) ) );
			}
		}

#if !defined( NDEBUG )
		auto it = result.begin();

		while ( it != result.end() )
		{
			auto nit = std::find_if( std::next( it )
				, result.end()
				, [it]( TextureUnitData * lookup )
				{
					return shallowEqual( ( *it )->passConfig.config, lookup->passConfig.config );
				} );
			bool ok = ( nit == result.end() );
			CU_Ensure( ok );
			++it;
		}
#endif
	}

	void Pass::prepareImage( TextureFlag flag
		, uint32_t maskOffset
		, uint32_t dstMask
		, castor::String const & name
		, TextureUnitDataSet & result )
	{
		auto & engine = *getOwner()->getEngine();
		auto & textureCache = engine.getTextureUnitCache();
		auto sources = matpass::getSources( m_sources, flag );

		if ( !sources.empty() )
		{
			auto & it = *sources.begin();
			auto animIt = m_animations.find( it.first );
			auto anim = ( animIt != m_animations.end()
				? std::move( animIt->second )
				: nullptr );
			result.emplace( &textureCache.getSourceData( it.first, it.second, std::move( anim ) ) );
		}
	}

	void Pass::setColour( castor::HdrRgbColour const & value )
	{
		for ( auto & component : m_components )
		{
			if ( component.second->hasColour() )
			{
				component.second->setColour( value );
			}
		}
	}

	castor::HdrRgbColour const & Pass::getColour()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->hasColour();
			} );
		static castor::HdrRgbColour const dummy{};
		return it == m_components.end()
			? dummy
			: it->second->getColour();
	}

	PassFlags Pass::getPassFlags()const
	{
		auto result = m_flags;

		for ( auto & component : m_components )
		{
			result |= component.second->getPassFlags();
		}

		return result;
	}

	void Pass::accept( PassVisitorBase & vis )
	{
		for ( auto & component : m_components )
		{
			component.second->accept( vis );
		}
	}

	void Pass::fillBuffer( PassBuffer & buffer
		, uint16_t passTypeIndex )const
	{
		if ( !getId() )
		{
			return;
		}

		getOwner()->getEngine()->getPassComponentsRegister().fillBuffer( *this, buffer );
	}

	PassSPtr Pass::clone( Material & material )const
	{
		auto result = std::make_unique< Pass >( material, getTypeID(), getPassFlags() );
		result->m_implicit = m_implicit;
		result->m_automaticShader = m_automaticShader;
		result->m_renderPassInfo = m_renderPassInfo;

		for ( auto & component : m_components )
		{
			result->addComponent( component.second->clone( *result ) );
		}

		for ( auto & source : m_sources )
		{
			auto it = m_animations.find( source.first );

			if ( it != m_animations.end()
				&& it->second )
			{
				auto & srcAnim = static_cast< TextureAnimation const & >( *it->second );
				auto clonedAnim = std::make_unique< TextureAnimation >( *srcAnim.getEngine()
					, srcAnim.getName() );
				clonedAnim->setRotateSpeed( srcAnim.getRotateSpeed() );
				clonedAnim->setScaleSpeed( srcAnim.getScaleSpeed() );
				clonedAnim->setTranslateSpeed( srcAnim.getTranslateSpeed() );
				result->registerTexture( source.first
					, source.second
					, std::move( clonedAnim ) );
			}
			else
			{
				result->registerTexture( source.first
					, source.second );
			}
		}

		result->prepareTextures();
		return result;
	}

	bool Pass::writeText( castor::String const & tabs
		, castor::Path const & folder
		, castor::String const & subfolder
		, castor::StringStream & file )const
	{
		bool result = true;

		for ( auto & component : m_components )
		{
			result = result
				&& component.second->writeText( tabs, folder, subfolder, file );
		}

		return result;
	}

	void Pass::fillConfig( TextureConfiguration & configuration
		, PassVisitorBase & vis )
	{
		for ( auto & component : m_components )
		{
			component.second->fillConfig( configuration, vis );
		}
	}

	void Pass::parseError( castor::String const & error )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream << cuT( "Error, : " ) << error;
		castor::Logger::logError( stream.str() );
	}

	void Pass::addParser( castor::AttributeParsers & parsers
		, uint32_t section
		, castor::String const & name
		, castor::ParserFunction function
		, castor::ParserParameterArray array
		, castor::String comment )
	{
		auto nameIt = parsers.find( name );

		if ( nameIt != parsers.end()
			&& nameIt->second.find( section ) != nameIt->second.end() )
		{
			parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
		}
		else
		{
			parsers[name][section] = { function, std::move( array ), std::move( comment ) };
		}
	}

	castor::RgbColour Pass::computeF0( castor::HdrRgbColour const & albedo
		, float metalness )
	{
		return castor::RgbColour{ matpass::mix( 0.04f, albedo.red(), metalness )
			, matpass::mix( 0.04f, albedo.green(), metalness )
			, matpass::mix( 0.04f, albedo.blue(), metalness ) };
	}

	float Pass::computeRoughnessFromGlossiness( float glossiness )
	{
		return 1.0f - glossiness;
	}

	float Pass::computeGlossinessFromRoughness( float roughness )
	{
		return 1.0f - roughness;
	}

	float Pass::computeGlossinessFromShininess( float shininess )
	{
		return shininess / MaxPhongShininess;
	}

	float Pass::computeShininessFromGlossiness( float glossiness )
	{
		return glossiness * MaxPhongShininess;
	}

	bool Pass::needsAlphaProcessing()const
	{
		bool result = checkFlag( m_textureFlags, TextureFlag::eOpacity );

		if ( auto comp = getComponent< OpacityComponent >() )
		{
			result = result && comp->needsAlphaProcessing();
		}

		return result;
	}

	BlendMode Pass::getAlphaBlendMode()const
	{
		if ( auto comp = getComponent< BlendComponent >() )
		{
			return comp->getAlphaBlendMode();
		}

		return BlendMode::eNoBlend;
	}

	BlendMode Pass::getColourBlendMode()const
	{
		if ( auto comp = getComponent< BlendComponent >() )
		{
			return comp->getColourBlendMode();
		}

		return BlendMode::eNoBlend;
	}

	VkCompareOp Pass::getAlphaFunc()const
	{
		if ( auto comp = getComponent< AlphaTestComponent >() )
		{
			return comp->getAlphaFunc();
		}

		return VK_COMPARE_OP_ALWAYS;
	}

	VkCompareOp Pass::getBlendAlphaFunc()const
	{
		if ( auto comp = getComponent< AlphaTestComponent >() )
		{
			return comp->getBlendAlphaFunc();
		}

		return VK_COMPARE_OP_ALWAYS;
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

	bool Pass::hasEnvironmentMapping()const
	{
		auto result = false;

		if ( auto comp = getComponent< ReflectionComponent >() )
		{
			result = comp->hasEnvironmentMapping();
		}

		if ( !result )
		{
			if ( auto comp = getComponent< RefractionComponent >() )
			{
				result = comp->hasEnvironmentMapping();
			}
		}

		return result;
	}

	bool Pass::hasSubsurfaceScattering()const
	{
		if ( auto comp = getComponent< SubsurfaceScatteringComponent >() )
		{
			return comp->hasSubsurfaceScattering();
		}

		return false;
	}

	bool Pass::isTwoSided()const
	{
		if ( auto comp = getComponent< TwoSidedComponent >() )
		{
			return comp->isTwoSided();
		}

		return false;
	}

	TextureUnitPtrArray Pass::getTextureUnits( TextureFlags mask )const
	{
		return matpass::getTextureUnits( m_textureUnits, mask );
	}

	uint32_t Pass::getTextureUnitsCount( TextureFlags mask )const
	{
		return uint32_t( matpass::getTextureUnits( m_textureUnits, mask ).size() );
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

	TextureFlags Pass::getTextures()const
	{
		return m_textureFlags;
	}

	bool Pass::hasLighting()const
	{
		if ( auto component = getComponent< PassHeaderComponent >() )
		{
			return component->isLightingEnabled();
		}

		return false;
	}

	void Pass::enableLighting( bool value )
	{
		if ( auto component = getComponent< PassHeaderComponent >() )
		{
			component->enableLighting( value );
		}
	}

	void Pass::doAddUnit( TextureUnitData & unitData
		, TextureUnitSPtr unit
		, TextureUnitPtrArray & result )
	{
		if ( unitData.passConfig.config.heightMask[0] )
		{
			m_heightTextureIndex = uint32_t( result.size() );
		}

		if ( unitData.animation && !unit->hasAnimation() )
		{
			auto anim = unitData.animation.get();
			
			unit->addAnimation( std::move( unitData.animation ) );
			static_cast< TextureAnimation & >( *anim ).setAnimable( *unit );
		}

		auto it = std::find_if( result.begin()
			, result.end()
			, [&unitData]( TextureUnitSPtr lookup )
			{
				return shallowEqual( unitData.passConfig.config, lookup->getConfiguration() );
			} );

		if ( it == result.end() )
		{
			result.push_back( unit );
		}
	}

	void Pass::doUpdateTextureFlags()
	{
		updateFlag( PassFlag::eAlphaBlending, hasAlphaBlending() );
		updateFlag( PassFlag::eAlphaTest, hasAlphaTest() || hasBlendAlphaTest() );

		if ( m_texturesReduced.exchange( false ) )
		{
			prepareTextures();
			onChanged( *this );
		}

		m_textureConfigs = TextureConfiguration{};

		for ( auto & source : m_sources )
		{
			mergeConfigs( source.second.config, m_textureConfigs );
		}

		getOwner()->getEngine()->getPassComponentsRegister().updateMapComponents( m_textureConfigs, *this );
		m_dirty = true;
	}
}
