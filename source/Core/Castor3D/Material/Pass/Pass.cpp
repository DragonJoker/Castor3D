#include "Castor3D/Material/Pass/Pass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Pass/Component/PassMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PickableComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileMappingComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/DefaultReflRefrComponent.hpp"
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
		using SortedTextureSources = std::map< PassComponentTextureFlag
			, Pass::PassTextureSource >;

		static SortedTextureSources sortSources( Pass::TextureSourceArray const & sources )
		{
			SortedTextureSources result;

			for ( auto & source : sources )
			{
 				result.emplace( *getFlags( source.first.textureConfig() ).begin()
					, source );
			}

			return result;
		}

		static Pass::TextureSourceArray::iterator removeConfiguration( PassComponentTextureFlag flag
			, TextureConfiguration config
			, Pass::TextureSourceArray::iterator it
			, Pass::TextureSourceArray & map )
		{
			if ( removeFlag( config, flag ) )
			{
				if ( config.components.end() == findFirstNonEmpty( config ) )
				{
					return map.erase( it );
				}

				auto passConfig = it->second;
				map.emplace_back( TextureSourceInfo{ it->first, config }, passConfig );
				return map.begin();
			}

			return std::next( it );
		}

		static TextureUnitDataRefs::iterator removeConfiguration( PassComponentTextureFlag flag
			, TextureConfiguration config
			, TextureUnitDataRefs::iterator it
			, TextureUnitDataRefs & array )
		{
			if ( removeFlag( config, flag ) )
			{
				if ( config.components.end() == findFirstNonEmpty( config ) )
				{
					return array.erase( it );
				}

				( *it )->base->sourceInfo = { ( *it )->base->sourceInfo, config };
			}

			return std::next( it );
		}

		static CU_ImplementAttributeParserNewBlock( parserPass, MaterialContext, PassContext )
		{
			if ( blockContext->material )
			{
				if ( blockContext->createMaterial
					|| blockContext->material->getPassCount() < blockContext->passIndex )
				{
					newBlockContext->pass = blockContext->material->createPass();
					newBlockContext->createPass = true;

				}
				else
				{
					newBlockContext->pass = blockContext->material->getPass( blockContext->passIndex );
					newBlockContext->createPass = false;
				}

				++blockContext->passIndex;
				newBlockContext->root = blockContext->root;
				newBlockContext->scene = blockContext->scene;
				newBlockContext->material = blockContext;
			}
			else
			{
				CU_ParsingError( cuT( "Material not initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::ePass )

		static CU_ImplementAttributeParserNewBlock( parserShader, PassContext, ProgramContext )
		{
			if ( blockContext->pass )
			{
				auto & cache = blockContext->pass->getOwner()->getEngine()->getShaderProgramCache();
				newBlockContext->shaderProgram = cache.getNewProgram( blockContext->material->material->getName() + castor::string::toString( blockContext->pass->getId() )
					, true );
			}
			else
			{
				CU_ParsingError( cuT( "Pass not initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eShaderProgram )

		static CU_ImplementAttributeParserBlock( parserEnd, PassContext )
		{
			if ( !blockContext->pass )
			{
				CU_ParsingError( cuT( "No Pass initialised." ) );
			}
			else
			{
				blockContext->pass->prepareTextures();
				log::info << "Loaded pass [" << blockContext->material->material->getName()
					<< ", " << blockContext->pass->getIndex() << "]" << std::endl;
			}
		}
		CU_EndAttributePop()
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
		if ( !m_initialised )
		{
			if ( !m_initialising.exchange( true ) )
			{
				UnitArray failed;

				for ( auto & unit : m_textureUnits )
				{
					if ( unit->failed() )
					{
						failed.push_back( unit );
					}
				}

				if ( !failed.empty() )
				{
					for ( auto & unit : failed )
					{
						auto it = std::find( m_textureUnits.begin()
							, m_textureUnits.end()
							, unit );
						m_textureUnits.erase( it );
					}

					std::vector< TextureFlagConfiguration > textureConfigs;

					for ( auto & unit : m_textureUnits )
					{
						for ( auto & component : unit->getConfiguration().components )
						{
							if ( component.componentsMask )
							{
								textureConfigs.push_back( component );
							}
						}
					}

					getPassComponentsRegister().updateMapComponents( textureConfigs, *this );
				}

				if ( m_reflRefrFlag == 0 )
				{
					createComponent< DefaultReflRefrComponent >();
				}

				m_componentCombine = getOwner()->getOwner()->getPassComponentsRegister().registerPassComponentCombine( *this );
				m_textureCombine = getOwner()->getOwner()->getTextureUnitCache().registerTextureCombine( *this );

				for ( auto & unit : m_textureUnits )
				{
					while ( !unit->isInitialised() && !unit->failed() )
					{
						std::this_thread::sleep_for( 1_ms );
					}
				}

				getOwner()->getOwner()->getMaterialCache().registerPass( *this );
				m_initialising = false;
				m_initialised = true;
			}
		}
	}

	void Pass::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			getOwner()->getOwner()->getMaterialCache().unregisterPass( *this );
		}
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

		if ( component->getPlugin().isReflRefrComponent() )
		{
			// Allow only one reflection/refraction component.
			if ( m_reflRefrFlag )
			{
				auto it = std::find_if( m_components.begin()
					, m_components.end()
					, [this]( PassComponentMap::value_type const & lookup )
					{
						return lookup.second->getPlugin().getComponentFlags() == m_reflRefrFlag;
					} );

				if ( it != m_components.end() )
				{
					m_components.erase( it );
				}
			}

			m_reflRefrFlag = {};
		}

		// Then add the component.
		auto id = getPassComponentsRegister().getNameId( component->getType() );

		if ( component->getPlugin().isMapComponent() )
		{
			if ( component->getPlugin().getColourMapFlags() )
			{
				m_colourMapFlag = component->getPlugin().getColourMapFlags();
			}

			if ( component->getPlugin().getOpacityMapFlags() )
			{
				m_opacityMapFlag = component->getPlugin().getOpacityMapFlags();
			}

			if ( component->getPlugin().getNormalMapFlags() )
			{
				m_normalMapFlag = component->getPlugin().getNormalMapFlags();
			}

			if ( component->getPlugin().getHeightMapFlags() )
			{
				m_heightMapFlag = component->getPlugin().getHeightMapFlags();
			}

			if ( component->getPlugin().getHeightMapFlags() )
			{
				m_occlusionMapFlag = component->getPlugin().getOcclusionMapFlags();
			}
		}

		if ( component->getPlugin().isReflRefrComponent() )
		{
			m_reflRefrFlag = component->getPlugin().getComponentFlags();
		}

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

			if ( tmp->getPlugin().getComponentFlags() == m_reflRefrFlag )
			{
				m_reflRefrFlag = {};
			}

			if ( tmp->getPlugin().isMapComponent() )
			{
				doRemoveConfiguration( static_cast< PassMapComponent & >( *tmp ).getTextureFlags() );

				if ( tmp->getPlugin().getTextureFlags() == m_colourMapFlag )
				{
					m_colourMapFlag = {};
				}

				if ( tmp->getPlugin().getTextureFlags() == m_opacityMapFlag )
				{
					m_opacityMapFlag = {};
				}

				if ( tmp->getPlugin().getTextureFlags() == m_normalMapFlag )
				{
					m_normalMapFlag = {};
				}

				if ( tmp->getPlugin().getTextureFlags() == m_heightMapFlag )
				{
					m_heightMapFlag = {};
				}

				if ( tmp->getPlugin().getTextureFlags() == m_occlusionMapFlag )
				{
					m_occlusionMapFlag = {};
				}
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
		CU_Require( m_textureCombine.baseId != 0u );
		return m_textureCombine.baseId;
	}

	TextureUnitRPtr Pass::getTextureUnit( uint32_t index )const
	{
		CU_Require( index < m_textureUnits.size() );
		return m_textureUnits[index];
	}

	void Pass::registerTexture( TextureSourceInfo sourceInfo
		, PassTextureConfig configuration )
	{
		auto it = std::find_if( m_sources.begin()
			, m_sources.end()
			, [&sourceInfo, &configuration]( PassTextureSource const & lookup )
			{
				return sourceInfo == lookup.first
					&& configuration.sampler == lookup.second.sampler
					&& configuration.texcoordSet == lookup.second.texcoordSet;
			} );

		if ( it == m_sources.end() )
		{
			m_sources.emplace_back( std::move( sourceInfo )
				, std::move( configuration ) );
		}
		else
		{
			auto resConfig = it->first.textureConfig();

			for ( auto & flagConfig : sourceInfo.textureConfig().components )
			{
				if ( flagConfig.flag )
				{
					addFlagConfiguration( resConfig, flagConfig );
				}
			}

			it->first = TextureSourceInfo{ it->first, resConfig };
		}

		m_maxTexcoordSet = std::max( m_maxTexcoordSet, configuration.texcoordSet );
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
		auto it = std::find_if( m_sources.begin()
			, m_sources.end()
			, [&sourceInfo]( PassTextureSource const & lookup )
			{
				return sourceInfo == lookup.first;
			} );

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
		auto it = std::find_if( m_sources.begin()
			, m_sources.end()
			, [&srcSourceInfo]( PassTextureSource const & lookup )
			{
				return srcSourceInfo == lookup.first;
			} );

		if ( it != m_sources.end() )
		{
			auto configuration = it->second;
			m_sources.erase( it );
			registerTexture( dstSourceInfo, configuration );
		}
	}

	void Pass::updateConfig( TextureSourceInfo const & sourceInfo
		, TextureConfiguration configuration )
	{
		resetTexture( sourceInfo
			, { sourceInfo, configuration } );
	}

	void Pass::prepareTextures()
	{
		if ( !m_texturesReduced.exchange( true ) )
		{
			auto sorted = matpass::sortSources( m_sources );
			m_prepared.clear();

			for ( auto & [flag, source] : sorted )
			{
				doPrepareImage( source );
			}

			m_textureUnits.clear();
			auto & engine = *getOwner()->getEngine();
			auto & textureCache = engine.getTextureUnitCache();

			for ( auto & prepared : m_prepared )
			{
				auto unit = textureCache.getTextureUnit( *prepared );
				doAddUnit( *prepared, unit, m_textureUnits );
			}

			m_textureCombine = getOwner()->getOwner()->getTextureUnitCache().registerTextureCombine( *this );
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

	PassComponentCombine Pass::getPassFlags()const
	{
		return m_componentCombine;
	}

	void Pass::accept( ConfigurationVisitorBase & vis )
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
		, ConfigurationVisitorBase & vis )
	{
		for ( auto & component : m_components )
		{
			component.second->fillConfig( configuration, vis );
		}
	}

	void Pass::addParsers( castor::AttributeParsers & result
			, castor::UInt32StrMap const & textureChannels )
	{
		using namespace castor;
		BlockParserContextT< MaterialContext > materialContext{ result, CSCNSection::eMaterial };
		BlockParserContextT< PassContext > passContext{ result, CSCNSection::ePass, CSCNSection::eMaterial };

		materialContext.addPushParser( cuT( "pass" ), CSCNSection::ePass, matpass::parserPass );
		passContext.addPushParser( cuT( "shader_program" ), CSCNSection::eShaderProgram, matpass::parserShader );
		passContext.addPopParser( cuT( "}" ), matpass::parserEnd );

		TextureUnit::addParsers( result, textureChannels );
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

	void Pass::doPrepareImage( PassTextureSource cfg )
	{
		auto & [sourceInfo, passConfig] = cfg;
		auto & engine = *getOwner()->getEngine();
		auto & textureCache = engine.getTextureUnitCache();
		auto animIt = m_animations.find( sourceInfo );
		auto anim = ( animIt != m_animations.end()
			? std::move( animIt->second )
			: nullptr );
		auto flags = getFlags( sourceInfo.textureConfig() );
		m_prepared.emplace_back( &textureCache.getSourceData( sourceInfo, passConfig, std::move( anim ) ) );
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
				return shallowEqual( unitData.base->sourceInfo.textureConfig(), lookup->getConfiguration() );
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
			onChanged( *this, m_componentCombine.baseId, m_componentCombine.baseId );

			for ( auto & prepared : m_prepared )
			{
				for ( auto & component : prepared->base->sourceInfo.textureConfig().components )
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
				for ( auto & component : source.first.textureConfig().components )
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
				, srcIt->first.textureConfig()
				, srcIt
				, m_sources );
		}

		auto datIt = m_prepared.begin();

		while ( datIt != m_prepared.end() )
		{
			datIt = matpass::removeConfiguration( flag
				, ( *datIt )->base->sourceInfo.textureConfig()
				, datIt
				, m_prepared );
		}

		doUpdateTextureFlags();
	}
}
