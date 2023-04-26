#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/PassVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PickableComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileMappingComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
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

CU_ImplementSmartPtr( castor3d, Pass )

namespace castor3d
{
	//*********************************************************************************************

	namespace matpass
	{
		static bool isPreparable( TextureSourceInfo const & source
			, PassTextureConfig const & config )
		{
			return ( !source.isRenderTarget() )
				&& !ashes::isCompressedFormat( config.imageInfo->format );
		}

		static bool isMergeable( TextureSourceInfo const & source
			, PassTextureConfig const & config
			, Animation const * animation )
		{
			return isPreparable( source, config )
				&& !animation;
		}

		static bool areFormatsCompatible( VkFormat lhs, VkFormat rhs )
		{
			if ( ashes::isCompressedFormat( lhs )
				|| ashes::isCompressedFormat( rhs ) )
			{
				return false;
			}

			if ( isSRGBFormat( convert( rhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( lhs ) ) );
			}

			if ( isSRGBFormat( convert( lhs ) ) )
			{
				return isSRGBFormat( getSRGBFormat( convert( rhs ) ) );
			}

			return true;
		}

		static bool areSpacesCompatible( TextureSpaces lhs, TextureSpaces rhs )
		{
			return checkFlag( lhs, TextureSpace::eAllowSRGB ) == checkFlag( rhs, TextureSpace::eAllowSRGB )
				&& checkFlag( lhs, TextureSpace::eNormalised ) == checkFlag( rhs, TextureSpace::eNormalised );
		}

		static bool areMergeable( std::unordered_map< TextureSourceInfo, TextureAnimationUPtr, TextureSourceInfoHasher > const & animations
			, Pass::PassTextureSource const & lhs
			, Pass::PassTextureSource const & rhs )
		{
			auto lhsFlags = getFlags( lhs.second.config );
			auto rhsFlags = getFlags( rhs.second.config );
			auto lhsAnimIt = animations.find( lhs.first );
			auto rhsAnimIt = animations.find( rhs.first );
			auto lhsAnim = ( lhsAnimIt != animations.end()
				? lhsAnimIt->second.get()
				: nullptr );
			auto rhsAnim = ( rhsAnimIt != animations.end()
				? rhsAnimIt->second.get()
				: nullptr );
			return lhsFlags.size() == 1u
				&& rhsFlags.size() == 1u
				&& lhs.second.texcoordSet == rhs.second.texcoordSet
				&& isMergeable( lhs.first, rhs.second, lhsAnim )
				&& isMergeable( lhs.first, rhs.second, rhsAnim )
				&& areFormatsCompatible( lhs.second.imageInfo->format, rhs.second.imageInfo->format )
				&& areSpacesCompatible( lhs.second.config.textureSpace, rhs.second.config.textureSpace );
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
				? castor::Path{ lhs.relative() }.getFileName()
				: castor::Path{ lhs.name() }.getFileName();
			auto rhsName = rhs.isFileImage()
				? castor::Path{ rhs.relative() }.getFileName()
				: castor::Path{ rhs.name() }.getFileName();
			auto name = castor::string::getLongestCommonSubstring( lhsName, rhsName );

			if ( name.size() > 2 )
			{
				castor::string::replace( lhsName, name, castor::String{} );
				castor::string::replace( rhsName, name, castor::String{} );
			}
			else
			{
				name.clear();
			}

			return TextureSourceInfo{ lhs.sampler()
				, folder
				, castor::Path{ name + lhsName + rhsName }
				, lhs.config() };
		}

		using SortedTextureSources = std::map< PassComponentTextureFlag
			, Pass::PassTextureSource >;

		static SortedTextureSources sortSources( TextureSourceMap const & sources )
		{
			SortedTextureSources result;

			for ( auto & source : sources )
			{
 				result.emplace( *getFlags( source.second.config ).begin()
					, source );
			}

			return result;
		}

		template< typename MapT, typename IterT >
		IterT removeConfiguration( PassComponentTextureFlag flag
			, TextureConfiguration & config
			, IterT it
			, MapT & map )
		{
			removeFlag( config, flag );

			if ( config.components.end() == findFirstNonEmpty( config ) )
			{
				return map.erase( it );
			}

			return std::next( it );
		}
	}

	//*********************************************************************************************

	Pass::Pass( Material & parent
		, LightingModelID lightingModelId )
		: OwnedBy< Material >{ parent }
		, m_index{ parent.getPassCount() }
		, m_renderPassInfo{ getOwner()->getRenderPassInfo() }
	{
		createComponent< BlendComponent >();
		createComponent< PassHeaderComponent >();
		createComponent< LightingModelComponent >()->setLightingModelId( lightingModelId );
		createComponent< TwoSidedComponent >();
		createComponent< PickableComponent >();
		createComponent< TexturesComponent >();
		createComponent< TextureCountComponent >();
		createComponent< ColourComponent >();
		createComponent< NormalComponent >();
	}

	Pass::Pass( Material & parent
		, Pass const & rhs )
		: Pass{ parent, rhs.getLightingModelId() }
	{
		m_implicit = rhs.m_implicit;
		m_automaticShader = rhs.m_automaticShader;
		m_renderPassInfo = rhs.m_renderPassInfo;

		for ( auto & component : rhs.m_components )
		{
			addComponent( component.second->clone( *this ) );
		}

		for ( auto & source : rhs.m_sources )
		{
			auto it = rhs.m_animations.find( source.first );

			if ( it != rhs.m_animations.end()
				&& it->second )
			{
				auto & srcAnim = static_cast< TextureAnimation const & >( *it->second );
				auto clonedAnim = castor::makeUnique< TextureAnimation >( *srcAnim.getEngine()
					, srcAnim.getName() );
				clonedAnim->setRotateSpeed( srcAnim.getRotateSpeed() );
				clonedAnim->setScaleSpeed( srcAnim.getScaleSpeed() );
				clonedAnim->setTranslateSpeed( srcAnim.getTranslateSpeed() );
				registerTexture( source.first
					, source.second
					, std::move( clonedAnim ) );
			}
			else
			{
				registerTexture( source.first
					, source.second );
			}
		}

		prepareTextures();
	}

	Pass::~Pass()noexcept
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

		m_componentCombine = getOwner()->getOwner()->getPassComponentsRegister().registerPassComponentCombine( *this );
		getOwner()->getOwner()->getMaterialCache().registerPass( *this );
	}

	void Pass::cleanup()
	{
		getOwner()->getOwner()->getMaterialCache().unregisterPass( *this );
	}

	void Pass::update()
	{
		for ( auto & component : m_components )
		{
			component.second->update();
		}

		if ( m_dirty.exchange( false ) )
		{
			auto oldComponents = m_componentCombine;
			m_componentCombine = getOwner()->getOwner()->getPassComponentsRegister().registerPassComponentCombine( *this );
			onChanged( *this, oldComponents.baseId, m_componentCombine.baseId );
		}
	}

	void Pass::addComponent( PassComponentUPtr component )
	{
		// First add the components this one depends on.
		for ( auto dep : component->getDependencies() )
		{
			if ( !hasComponent( dep ) )
			{
				addComponent( getOwner()->getOwner()->getPassComponentsRegister().getPlugin( dep ).createComponent( *this ) );
			}
		}

		// Then add the component.
		auto id = getPassComponentsRegister().getNameId( component->getType() );
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

	std::vector< PassComponentUPtr > Pass::removeComponent( castor::String const & name )
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&name]( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getType() == name;
			} );
		std::vector< PassComponentUPtr > result;

		if ( it != m_components.end() )
		{
			auto tmp = std::move( it->second );
			m_components.erase( it );

			if ( tmp->getPlugin().isMapComponent() )
			{
				doRemoveConfiguration( static_cast< PassMapComponent & >( *tmp ).getTextureFlags() );
			}

			result = doRemoveDependencies( name );
			result.emplace_back( std::move( tmp ) );
			m_dirty = true;
		}

		return result;
	}

	shader::PassMaterialShader * Pass::getMaterialShader( castor::String const & componentType )const
	{
		return getPassComponentsRegister().getMaterialShader( componentType );
	}

	PassComponentID Pass::getComponentId( castor::String const & componentType )const
	{
		return getPassComponentsRegister().getNameId( componentType );
	}

	PassComponentPlugin const & Pass::getComponentPlugin( PassComponentID componentId )const
	{
		return getPassComponentsRegister().getPlugin( componentId );
	}

	PassComponentCombineID Pass::getComponentCombineID()const
	{
		CU_Require( m_componentCombine.baseId != 0u );
		return m_componentCombine.baseId;
	}

	TextureCombineID Pass::getTextureCombineID()const
	{
		return getPassComponentsRegister().getTextureCombineID( m_textureCombine );
	}

	TextureUnitRPtr Pass::getTextureUnit( uint32_t index )const
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
			m_sources.emplace( std::move( sourceInfo )
				, std::move( configuration ) );
		}

		doUpdateTextureFlags();
	}

	void Pass::registerTexture( TextureSourceInfo sourceInfo
		, PassTextureConfig configuration
		, TextureAnimationUPtr animation )
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
			
			doUpdateTextureFlags();
		}
	}

	void Pass::updateConfig( TextureSourceInfo const & sourceInfo
		, TextureConfiguration configuration )
	{
		auto it = m_sources.find( sourceInfo );

		if ( it != m_sources.end() )
		{
			it->second.config = std::move( configuration );
			doUpdateTextureFlags();
		}
		else
		{
			auto srcIt = std::find_if( m_realSources.begin()
				, m_realSources.end()
				, [&sourceInfo]( auto & lookup )
				{
					return sourceInfo == lookup.first->sourceInfo;
				} );

			if ( srcIt != m_realSources.end() )
			{
				srcIt->first->passConfig.config = std::move( configuration );
				doUpdateTextureFlags();
			}
		}
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced.exchange( true ) )
		{
			auto oldSources = std::move( m_realSources );
			//
			//	Sort per used components, decrementally
			//
			auto remaining = matpass::sortSources( m_sources );
			//
			//		Four and two component images are not merged with others,
			//		hence get rid of them first
			//
			auto it = remaining.begin();
			while ( it != remaining.end() )
			{
				if ( getPixelComponents( it->second.second.config ).size() == 4u
					|| getPixelComponents( it->second.second.config ).size() == 2u )
				{
					doPrepareImage( std::move( it->second ) );
					it = remaining.erase( it );
				}
				else
				{
					++it;
				}
			}
			//
			//		Then three components
			//
			it = remaining.begin();
			while ( it != remaining.end() )
			{
				if ( getPixelComponents( it->second.second.config ).size() == 3u )
				{
					PassTextureSource lhs = std::move( it->second );
					remaining.erase( it );

					if ( getFlags( lhs.second.config ).size() == 1u )
					{
						//
						//	Fill with a one component texture
						//
						it = std::find_if( remaining.begin()
							, remaining.end()
							, [this, &lhs]( matpass::SortedTextureSources::value_type const & lookup )
							{
								return getPixelComponents( lookup.second.second.config ).size() == 1u
									&& matpass::areMergeable( m_animations, lhs, lookup.second );
							} );

						if ( it == remaining.end() )
						{
							doPrepareImage( std::move( lhs ) );
						}
						else
						{
							doMergeImages( std::move( lhs ), 0x00FFFFFFu
								, std::move( it->second ), 0xFF000000u
								, oldSources );
							remaining.erase( it );
						}
					}
					else
					{
						doPrepareImage( lhs );
					}

					it = remaining.begin();
				}
				else
				{
					++it;
				}
			}
			//
			//		Then one component
			//
			it = remaining.begin();
			while ( it != remaining.end() )
			{
				CU_Require( getPixelComponents( it->second.second.config ).size() == 1u );
				PassTextureSource lhs = std::move( it->second );
				remaining.erase( it );
				//
				//	Group them by two.
				//
				it = std::find_if( remaining.begin()
					, remaining.end()
					, [this, &lhs]( matpass::SortedTextureSources::value_type const & lookup )
					{
						CU_Require( getPixelComponents( lookup.second.second.config ).size() == 1u );
						return matpass::areMergeable( m_animations, lhs, lookup.second );
					} );

				if ( it == remaining.end() )
				{
					doPrepareImage( std::move( lhs ) );
				}
				else
				{
					doMergeImages( std::move( lhs ), 0x00FF0000u
						, std::move( it->second ), 0x0000FF00u
						, oldSources );
					remaining.erase( it );
				}

				it = remaining.begin();
			}

			Pass::UnitArray newUnits;

			// Then add the other ones.
			for ( auto & source : m_realSources )
			{
				auto & textureCache = getOwner()->getEngine()->getTextureUnitCache();
				auto unit = textureCache.getTexture( *source.first );
				doAddUnit( *source.first, unit, newUnits );
			}

			m_textureUnits = newUnits;
			m_textureCombine = getOwner()->getOwner()->getTextureUnitCache().registerTextureCombine( *this );
		}
	}

	void Pass::doMergeImages( PassTextureSource lhsIt
		, uint32_t lhsDstMask
		, PassTextureSource rhsIt
		, uint32_t rhsDstMask
		, UnitDataSources const & configs )
	{
		auto & engine = *getOwner()->getEngine();
		auto & textureCache = engine.getTextureUnitCache();
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

		auto lhsFlags = getEnabledFlag( lhsIt.second.config );
		auto rhsFlags = getEnabledFlag( rhsIt.second.config );
		auto name = getTextureFlagsName( lhsFlags ) + getTextureFlagsName( rhsFlags );

		log::debug << getOwner()->getName() << name << cuT( " - Merging textures." ) << std::endl;
		auto resultSourceInfo = matpass::mergeSourceInfos( lhsIt.first, rhsIt.first );
		PassTextureConfig resultConfig;
		resultConfig.config.heightFactor = std::min( lhsIt.second.config.heightFactor
			, rhsIt.second.config.heightFactor );
		addFlagConfiguration( resultConfig.config, { lhsFlags, lhsDstMask } );
		addFlagConfiguration( resultConfig.config, { rhsFlags, rhsDstMask } );
		mergeConfigsBase( lhsIt.second.config, resultConfig.config );
		mergeConfigsBase( rhsIt.second.config, resultConfig.config );
		auto flags = getFlags( resultConfig.config );
		auto ires = m_realSources.emplace( &textureCache.mergeSources( lhsIt.first
				, lhsIt.second
				, getComponentsMask( lhsIt.second.config, lhsFlags )
				, lhsDstMask
				, rhsIt.first
				, rhsIt.second
				, getComponentsMask( rhsIt.second.config, rhsFlags )
				, rhsDstMask
				, getOwner()->getName() + name
				, resultSourceInfo
				, resultConfig )
			, std::vector< TextureSourceInfo >{ lhsIt.first, rhsIt.first } );
		auto configIt = configs.find( ires.first->first );

		if ( configs.end() != configIt )
		{
			ires.first->first->passConfig.config = configIt->first->passConfig.config;
		}

#if !defined( NDEBUG )
		auto it = m_realSources.begin();

		while ( it != m_realSources.end() )
		{
			auto nit = std::find_if( std::next( it )
				, m_realSources.end()
				, [it]( auto & lookup )
				{
					return shallowEqual( it->first->passConfig.config, lookup.first->passConfig.config );
				} );
			bool ok = ( nit == m_realSources.end() );
			CU_Ensure( ok );
			++it;
		}
#endif
	}

	void Pass::doPrepareImage( PassTextureSource cfg )
	{
		auto & engine = *getOwner()->getEngine();
		auto & textureCache = engine.getTextureUnitCache();
		auto animIt = m_animations.find( cfg.first );
		auto anim = ( animIt != m_animations.end()
			? std::move( animIt->second )
			: nullptr );
		auto flags = getFlags( cfg.second.config );
		m_realSources.emplace( &textureCache.getSourceData( cfg.first, cfg.second, std::move( anim ) )
			, std::vector< TextureSourceInfo >{ cfg.first } );
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

	PassComponentCombine Pass::getPassFlags()const
	{
		return m_componentCombine;
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

		getPassComponentsRegister().fillBuffer( *this, buffer );
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
		bool result = hasAny( getTexturesMask(), getOpacityMapFlags() );

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
			result = hasComponent< RefractionComponent >()
				&& !hasComponent< TransmissionComponent >();
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

	Pass::UnitArray Pass::getTextureUnits()const
	{
		return m_textureUnits;
	}

	uint32_t Pass::getTextureUnitsCount()const
	{
		return uint32_t( m_textureUnits.size() );
	}

	TextureCombine Pass::getTexturesMask()const
	{
		return m_textureCombine;
	}

	bool Pass::hasLighting()const
	{
		if ( auto component = getComponent< PassHeaderComponent >() )
		{
			return component->isLightingEnabled();
		}

		return false;
	}

	PassComponentRegister & Pass::getPassComponentsRegister()const
	{
		return getOwner()->getEngine()->getPassComponentsRegister();
	}

	PassComponentTextureFlag Pass::getColourMapFlags()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& lookup.second->getPlugin().getColourMapFlags() != 0u;
			} );
		return it == m_components.end()
			? getPassComponentsRegister().getColourMapFlags()
			: it->second->getPlugin().getColourMapFlags();
	}

	PassComponentTextureFlag Pass::getOpacityMapFlags()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& lookup.second->getPlugin().getOpacityMapFlags() != 0u;
			} );
		return it == m_components.end()
			? getPassComponentsRegister().getOpacityMapFlags()
			: it->second->getPlugin().getOpacityMapFlags();
	}

	PassComponentTextureFlag Pass::getNormalMapFlags()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& lookup.second->getPlugin().getNormalMapFlags() != 0u;
			} );
		return it == m_components.end()
			? getPassComponentsRegister().getNormalMapFlags()
			: it->second->getPlugin().getNormalMapFlags();
	}

	PassComponentTextureFlag Pass::getHeightMapFlags()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& lookup.second->getPlugin().getHeightMapFlags() != 0u;
			} );
		return it == m_components.end()
			? getPassComponentsRegister().getHeightMapFlags()
			: it->second->getPlugin().getHeightMapFlags();
	}

	PassComponentTextureFlag Pass::getOcclusionMapFlags()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& lookup.second->getPlugin().getOcclusionMapFlags() != 0u;
			} );
		return it == m_components.end()
			? getPassComponentsRegister().getOcclusionMapFlags()
			: it->second->getPlugin().getOcclusionMapFlags();
	}

	castor::String Pass::getTextureFlagsName( PassComponentTextureFlag flags )const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, [&flags]( PassComponentMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().isMapComponent()
					&& !lookup.second->getPlugin().getTextureFlagsName( flags ).empty();
			} );
		return it == m_components.end()
			? castor::String{}
			: it->second->getPlugin().getTextureFlagsName( flags );
	}

	LightingModelID Pass::getLightingModelId()const
	{
		LightingModelID result{};

		if ( auto component = getComponent< LightingModelComponent >() )
		{
			result = component->getLightingModelId();
		}

		return result;
	}

	void Pass::enableLighting( bool value )
	{
		if ( auto component = getComponent< PassHeaderComponent >() )
		{
			component->enableLighting( value );
		}
	}

	void Pass::enablePicking( bool value )
	{
		if ( auto component = getComponent< PickableComponent >() )
		{
			component->setPickable( value );
		}
	}

	void Pass::doAddUnit( TextureUnitData & unitData
		, TextureUnitRPtr unit
		, Pass::UnitArray & result )
	{
		if ( unitData.animation && !unit->hasAnimation() )
		{
			auto anim = unitData.animation.get();
			unit->addAnimation( castor::ptrRefCast< Animation >( unitData.animation ) );
			static_cast< TextureAnimation & >( *anim ).setAnimable( *unit );
		}

		auto it = std::find_if( result.begin()
			, result.end()
			, [&unitData]( TextureUnitRPtr lookup )
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
		std::vector< TextureFlagConfiguration > textureConfigs;

		if ( m_texturesReduced.exchange( false ) )
		{
			prepareTextures();
			m_textureCombine = getOwner()->getOwner()->getTextureUnitCache().registerTextureCombine( *this );
			onChanged( *this, m_componentCombine.baseId, m_componentCombine.baseId );

			for ( auto & source : m_realSources )
			{
				for ( auto & component : source.first->passConfig.config.components )
				{
					if ( component.componentsMask )
					{
						textureConfigs.push_back( component );
					}
				}
			}
		}
		else
		{
			for ( auto & source : m_sources )
			{
				for ( auto & component : source.second.config.components )
				{
					if ( component.componentsMask )
					{
						textureConfigs.push_back( component );
					}
				}
			}
		}

		getPassComponentsRegister().updateMapComponents( textureConfigs, *this );
		m_dirty = true;
	}

	std::vector< PassComponentUPtr > Pass::doRemoveDependencies( castor::String const & name )
	{
		std::vector< PassComponentUPtr > result;
		// First gather the ones depending directly from it.
		castor::StringArray depends;

		for ( auto & [id, component] : m_components )
		{
			auto & compDeps = component->getDependencies();
			auto compIt = std::find_if( compDeps.begin()
				, compDeps.end()
				, [&name]( castor::String const & lookup )
				{
					return lookup == name;
				} );

			if ( compIt != compDeps.end() )
			{
				depends.push_back( component->getType() );
			}
		}

		// Then recursively remove them.
		for ( auto dep : depends )
		{
			auto removed = removeComponent( dep );

			for ( auto & rem : removed )
			{
				result.emplace_back( std::move( rem ) );
			}
		}

		return result;
	}

	void Pass::doRemoveConfiguration( PassComponentTextureFlag flag )
	{
		auto srcIt = m_sources.begin();

		while ( srcIt != m_sources.end() )
		{
			srcIt = matpass::removeConfiguration( flag
				, srcIt->second.config
				, srcIt
				, m_sources );
		}

		auto datIt = m_realSources.begin();

		while ( datIt != m_realSources.end() )
		{
			datIt = matpass::removeConfiguration( flag
				, datIt->first->passConfig.config
				, datIt
				, m_realSources );
		}

		doUpdateTextureFlags();
	}
}
