#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/FractalMappingComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PickableComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileMappingComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SheenComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/ThicknessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmittanceComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/AttenuationMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/IridescenceMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/IridescenceThicknessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SheenMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SheenRoughnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/TransmissionMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/DefaultReflRefrComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"

CU_ImplementSmartPtr( castor3d, PassComponentRegister )

namespace castor3d
{
	namespace passcompreg
	{
		static constexpr PassComponentID InvalidId = PassComponentID( ~PassComponentID{} );

		static bool isValidComponent( PipelineFlags const & flags
			, PassComponentID componentId )
		{
			return flags.pass.flags.end() != std::find_if( flags.pass.flags.begin()
				, flags.pass.flags.end()
				, [&componentId]( PassComponentFlag const & lookup )
				{
					auto lookupComponentId = splitPassComponentFlag( lookup ).first;
					return lookupComponentId == componentId;
				} );
		}

		static bool isValidComponent( TextureCombine const &
			, PassComponentID )
		{
			return true;
		}

		static TextureCombine getTextureFlags( PipelineFlags const & flags )
		{
			return flags.textures;
		}

		static TextureCombine getTextureFlags( TextureCombine const & flags )
		{
			return flags;
		}

		template< typename FlagsT >
		void getComponentShadersT( FlagsT const & flags
			, ComponentModeFlags filter
			, PassComponentID componentId
			, PassComponentPlugin const & component
			, castor::Vector< shader::PassComponentsShaderPtr > & shaders
			, castor::Vector< UpdateComponent > & updateComponents
			, castor::Vector< FinishComponent > & finishComponents )
		{
			if ( isValidComponent( flags, componentId ) )
			{
				if ( component.isComponentNeeded( getTextureFlags( flags ), filter ) )
				{
					if ( auto shader = component.createComponentsShader() )
					{
						shaders.push_back( castor::move( shader ) );
					}

					if ( component.updateComponent != nullptr )
					{
						updateComponents.push_back( component.updateComponent );
					}

					if ( component.finishComponent != nullptr )
					{
						finishComponents.push_back( component.finishComponent );
					}
				}
			}
			else
			{
				if ( component.updateComponent != nullptr )
				{
					updateComponents.push_back( component.updateComponent );
				}

				if ( component.finishComponent != nullptr )
				{
					finishComponents.push_back( component.finishComponent );
				}
			}
		}
	}

	PassComponentRegister::PassComponentRegister( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
	{
		// 
		// Order is important here, for priority.
		// Especially height, opacity and normal which must start
		// the image related components.
		// 

		// Pass base configuration
		registerComponent< PassHeaderComponent >();
		registerComponent< LightingModelComponent >();
		registerComponent< TexturesComponent >();
		registerComponent< BlendComponent >();
		registerComponent< TwoSidedComponent >();
		registerComponent< PickableComponent >();
		registerComponent< UntileMappingComponent >();
		registerComponent< FractalMappingComponent >();
		registerComponent< NormalComponent >();
		registerComponent< DefaultReflRefrComponent >();
		// Pass shader buffer components
		registerComponent< HeightComponent >();
		registerComponent< OpacityComponent >();
		registerComponent< ColourComponent >();
		registerComponent< AmbientComponent >();
		registerComponent< EmissiveComponent >();
		registerComponent< TransmissionComponent >();
		registerComponent< AttenuationComponent >();
		registerComponent< ThicknessComponent >();
		registerComponent< RefractionComponent >();
		registerComponent< MetalnessComponent >();
		registerComponent< SpecularComponent >();
		registerComponent< SpecularFactorComponent >();
		registerComponent< RoughnessComponent >();
		registerComponent< AlphaTestComponent >();
		registerComponent< ReflectionComponent >();
		registerComponent< ClearcoatComponent >();
		registerComponent< SheenComponent >();
		registerComponent< IridescenceComponent >();
		registerComponent< TransmittanceComponent >();
		registerComponent< SubsurfaceScatteringComponent >();
		// Pass shader image components
		registerComponent< HeightMapComponent >();
		registerComponent< NormalMapComponent >();
		registerComponent< OpacityMapComponent >();
		registerComponent< ColourMapComponent >();
		registerComponent< EmissiveMapComponent >();
		registerComponent< OcclusionMapComponent >();
		registerComponent< SpecularMapComponent >();
		registerComponent< SpecularFactorMapComponent >();
		registerComponent< MetalnessMapComponent >();
		registerComponent< GlossinessMapComponent >();
		registerComponent< RoughnessMapComponent >();
		registerComponent< TransmittanceMapComponent >();
		registerComponent< TransmissionMapComponent >();
		registerComponent< AttenuationMapComponent >();
		registerComponent< ClearcoatMapComponent >();
		registerComponent< ClearcoatNormalMapComponent >();
		registerComponent< ClearcoatRoughnessMapComponent >();
		registerComponent< SheenMapComponent >();
		registerComponent< SheenRoughnessMapComponent >();
		registerComponent< IridescenceMapComponent >();
		registerComponent< IridescenceThicknessMapComponent >();

		m_pauseOrder = false;
		reorderBuffer();

		PassComponentCombine dummy{};
		registerPassComponentCombine( dummy );
	}

	PassComponentRegister::~PassComponentRegister()noexcept
	{
		m_pauseOrder = true;

		while ( !m_registered.empty() )
		{
			if ( auto const & componentDesc = m_registered.back();
				componentDesc.plugin )
			{
				unregisterComponent( componentDesc.name );
			}

			m_registered.pop_back();
		}
	}

	PassComponentCombine PassComponentRegister::registerPassComponentCombine( Pass const & pass )
	{
		PassComponentCombine result{};

		for ( auto const & [id, component] : pass.getComponents() )
		{
			addFlags( result, component->getPassFlags() );
		}

		registerPassComponentCombine( result );
		return result;
	}

	PassComponentCombineID PassComponentRegister::registerPassComponentCombine( PassComponentCombine & combine )
	{
		auto it = std::find_if( m_componentCombines.begin()
			, m_componentCombines.end()
			, [&combine]( PassComponentCombine const & lookup )
			{
				return lookup.flags == combine.flags;
			} );

		if ( it == m_componentCombines.end() )
		{
			m_componentCombines.push_back( combine );
			auto idx = SubmeshComponentCombineID( m_componentCombines.size() - 1u );
			it = std::next( m_componentCombines.begin(), idx );
			it->baseId = idx + 1u;
			fillPassComponentCombine( *it );
		}

		if ( it->baseId > MaxPassCombines )
		{
			CU_Failure( "Overflown pass combines count." );
			CU_Exception( "Overflown pass combines count." );
		}

		fillPassComponentCombine( combine );
		combine.baseId = it->baseId;
		return combine.baseId;
	}

	PassComponentCombineID PassComponentRegister::getPassComponentCombineID( PassComponentCombine const & combine )const
	{
		if ( combine.baseId == 0u )
		{
			auto it = std::find( m_componentCombines.begin()
				, m_componentCombines.end()
				, combine );

			if ( it == m_componentCombines.end() )
			{
				CU_Failure( "Pass components combination was not registered." );
				CU_Exception( "Pass components combination was not registered." );
			}

			return PassComponentCombineID( std::distance( m_componentCombines.begin(), it ) + 1 );
		}

		CU_Require( combine.baseId <= m_componentCombines.size() );
		return combine.baseId;
	}

	TextureCombineID PassComponentRegister::getTextureCombineID( TextureCombine const & combine )const
	{
		return getOwner()->getTextureUnitCache().getTextureCombineID( combine );
	}

	PassComponentCombine PassComponentRegister::getPassComponentCombine( Pass const & pass )const
	{
		return getPassComponentCombine( pass.getComponentCombineID() );
	}

	PassComponentCombine PassComponentRegister::getPassComponentCombine( PassComponentCombineID id )const
	{
		CU_Require( id <= m_componentCombines.size() );

		if ( id == 0 )
		{
			CU_Failure( "Unexpected invalid pass type." );
			return PassComponentCombine{};
		}

		return m_componentCombines[id - 1ULL];
	}

	TextureCombine PassComponentRegister::getTextureCombine( Pass const & pass )const
	{
		return getTextureCombine( pass.getTextureCombineID() );
	}

	TextureCombine PassComponentRegister::getTextureCombine( TextureCombineID id )const
	{
		return getOwner()->getTextureUnitCache().getTextureCombine( id );
	}

	void PassComponentRegister::fillBuffer( Pass const & pass
		, PassBuffer & buffer )const
	{
		for ( auto & id : m_bufferOrder )
		{
			auto & componentDesc = m_registered[id - 1ULL];
			CU_Require( id == componentDesc.id );

			if ( auto component = pass.getComponent( componentDesc.name ) )
			{
				component->fillBuffer( buffer );
			}
			else
			{
				componentDesc.plugin->zeroBuffer( pass
					, *m_materialShaders.find( id )->second
					, buffer );
			}
		}
	}

	shader::PassMaterialShader * PassComponentRegister::getMaterialShader( castor::String const & componentType )const
	{
		auto it = m_materialShaders.find( getNameId( componentType ) );
		return it == m_materialShaders.end()
			? nullptr
			: it->second.get();
	}

	void PassComponentRegister::fillMaterial( sdw::type::BaseStruct & material
		, sdw::expr::ExprList & inits
		, uint32_t padIndex )const
	{
		for ( auto & fill : m_fillMaterial )
		{
			fill( material, inits, padIndex );
		}
	}

	castor::Vector< shader::PassComponentsShaderPtr > PassComponentRegister::getComponentsShaders( TextureCombine const & combine
		, ComponentModeFlags filter
		, castor::Vector< UpdateComponent > & updateComponents
		, castor::Vector< FinishComponent > & finishComponents )const
	{
		castor::Vector< shader::PassComponentsShaderPtr > result;

		for ( auto & componentDesc : m_registered )
		{
			passcompreg::getComponentShadersT( combine
				, filter
				, componentDesc.id
				, *componentDesc.plugin
				, result
				, updateComponents
				, finishComponents );
		}

		return result;
	}

	castor::Vector< shader::PassComponentsShaderPtr > PassComponentRegister::getComponentsShaders( PipelineFlags const & flags
		, ComponentModeFlags filter
		, castor::Vector< UpdateComponent > & updateComponents
		, castor::Vector< FinishComponent > & finishComponents )const
	{
		castor::Vector< shader::PassComponentsShaderPtr > result;

		for ( auto & componentDesc : m_registered )
		{
			passcompreg::getComponentShadersT( flags
				, filter
				, componentDesc.id
				, *componentDesc.plugin
				, result
				, updateComponents
				, finishComponents );
		}

		return result;
	}

	shader::PassReflRefrShaderPtr PassComponentRegister::getReflRefrShader( PassComponentCombine const & combine )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&combine]( Component const & lookup )
			{
				return lookup.plugin->isReflRefrComponent()
					&& hasAny( combine, lookup.plugin->getComponentFlags() );
			} );

		if ( it == m_registered.end() )
		{
			CU_Failure( "Pass components combination doesn't contain any reflection/refraction component" );
			CU_Exception( "Pass components combination doesn't contain any reflection/refraction component" );
		}

		return it->plugin->createReflRefrShader();
	}

	PassComponentCombine PassComponentRegister::filterComponentFlags( ComponentModeFlags filter
		, PassComponentCombine const & combine )const
	{
		PassComponentCombine result{ combine };

		for ( auto & componentDesc : m_registered )
		{
			componentDesc.plugin->filterComponentFlags( filter, result );
		}

		return result;
	}

	bool PassComponentRegister::hasOpacity( PipelineFlags const & flags )const
	{
		return ( hasAny( flags.pass, getAlphaBlendingFlag() )
			|| ( hasAny( flags.pass, getAlphaTestFlag() ) && ( flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) ) );
	}

	bool PassComponentRegister::needsEnvironmentMapping( PassComponentCombineID combineID )const
	{
		auto components = getPassComponentCombine( combineID );
		return ( hasAny( components, getPlugin( ReflectionComponent::TypeName ).getComponentFlags() )
			|| ( hasAny( components, getPlugin( RefractionComponent::TypeName ).getComponentFlags() )
				&& !hasAny( components, getPlugin( TransmissionComponent::TypeName ).getComponentFlags() ) ) );
	}

	bool PassComponentRegister::hasDeferredLighting( PassComponentFlagsSet const & components )const
	{
		return components.end() != components.find( getPlugin( SubsurfaceScatteringComponent::TypeName ).getComponentFlags() );
	}

	void PassComponentRegister::updateMapComponents( castor::Vector< TextureFlagConfiguration > const & texConfigs
		, Pass & result )
	{
		castor::Vector< PassComponentUPtr > components;
		castor::Map< PassComponentID, Component const * > needed;

		// First gather the needed map components.
		for ( auto & texConfig : texConfigs )
		{
			for ( auto & componentDesc : m_registered )
			{
				if ( componentDesc.plugin->isMapComponent()
					&& hasIntersect( texConfig.flag, componentDesc.plugin->getTextureFlags() ) )
				{
					needed.emplace( componentDesc.id, &componentDesc );
				}
			}
		}

		// Then update the pass map components given the needed ones list.
		for ( auto const & componentDesc : m_registered )
		{
			if ( componentDesc.plugin->isMapComponent() )
			{
				auto needit = needed.find( componentDesc.id );
				bool hasComponent = result.hasComponent( componentDesc.name );

				if ( needit != needed.end() )
				{
					if ( !hasComponent )
					{
						componentDesc.plugin->createMapComponent( result, components );
					}
				}
				else if ( hasComponent )
				{
					result.removeComponent( componentDesc.name );
				}
			}
		}

		for ( auto & component : components )
		{
			result.addComponent( castor::move( component ) );
		}
	}

	bool PassComponentRegister::writeTextureConfig( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file )const
	{
		bool result = true;

		for ( auto & componentDesc : m_registered )
		{
			result = result
				&& componentDesc.plugin->writeTextureConfig( configuration, tabs, file );
		}

		return result;
	}

	void PassComponentRegister::fillChannels( PassComponentTextureFlag const & flags
		, TextureContext & parsingContext )const
	{
		for ( auto & [_, channelFiller] : m_channelsFillers )
		{
			if ( flags == channelFiller.first )
			{
				channelFiller.second( parsingContext );
			}
		}
	}

	TextureCombine PassComponentRegister::filterTextureFlags( ComponentModeFlags filter
		, TextureCombine const & combine )const
	{
		TextureCombine result{ combine };

		for ( auto & componentDesc : m_registered )
		{
			componentDesc.plugin->filterTextureFlags( filter, result );
		}

		result.configCount = result.flags.empty()
			? 0u
			: result.configCount;
		getEngine()->getTextureUnitCache().registerTextureCombine( result );
		return result;
	}

	PassComponentTextureFlag PassComponentRegister::getColourMapFlags()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& lookup.plugin->getColourMapFlags() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getColourMapFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getOpacityMapFlags()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& lookup.plugin->getOpacityMapFlags() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getOpacityMapFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getNormalMapFlags()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& lookup.plugin->getNormalMapFlags() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getNormalMapFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getHeightMapFlags()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& lookup.plugin->getHeightMapFlags() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getHeightMapFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getOcclusionMapFlags()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& lookup.plugin->getOcclusionMapFlags() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getOcclusionMapFlags();
	}

	void PassComponentRegister::fillTextureConfiguration( PassComponentTextureFlag const & flags
		, TextureConfiguration & result )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&flags]( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& hasIntersect( flags, lookup.plugin->getTextureFlags() );
			} );

		if ( it != m_registered.end() )
		{
			it->plugin->fillTextureConfiguration( result );
		}
	}

	bool PassComponentRegister::hasTexcoordModif( PassComponentTextureFlag const & flag
		, PipelineFlags const * flags )const
	{
		return m_registered.end() != std::find_if( m_registered.begin()
			, m_registered.end()
			, [this, &flag, flags]( Components::value_type const & lookup )
			{
				return lookup.plugin->isMapComponent()
					&& hasIntersect( flag, lookup.plugin->getTextureFlags() )
					&& lookup.plugin->hasTexcoordModif( *this, flags );
			} );
	}

	castor::Map< uint32_t, PassComponentTextureFlag > PassComponentRegister::getTexcoordModifs( PipelineFlags const & flags )const
	{
		auto & textures = flags.textures;
		castor::Map< uint32_t, PassComponentTextureFlag > result;
		uint32_t index = 0u;

		for ( auto flag : textures.flags )
		{
			if ( hasTexcoordModif( flag, &flags ) )
			{
				result.emplace( index, flag );
			}
		}

		return result;
	}

	castor::Map< uint32_t, PassComponentTextureFlag > PassComponentRegister::getTexcoordModifs( TextureCombine const & combine )const
	{
		castor::Map< uint32_t, PassComponentTextureFlag > result;
		uint32_t index = 0u;

		for ( auto flag : combine.flags )
		{
			if ( hasTexcoordModif( flag, nullptr ) )
			{
				result.emplace( index, flag );
			}
		}

		return result;
	}

	PassComponentID PassComponentRegister::registerComponent( castor::String const & componentType
		, PassComponentPluginUPtr componentPlugin )
	{
		if ( auto id = getNameId( componentType );
			id != passcompreg::InvalidId )
		{
			log::error << "Pass component type [" << componentType << "] is already registered." << std::endl;
			CU_Failure( "Pass component type is already registered" );
			return id;
		}

		auto & componentDesc = getNextId();
		registerComponent( componentDesc
			, componentType
			, castor::move( componentPlugin ) );
		return componentDesc.id;
	}

	void PassComponentRegister::unregisterComponent( castor::String const & componentType )noexcept
	{
		auto id = getNameId( componentType );

		if ( id == passcompreg::InvalidId )
		{
			log::error << "Component type [" << componentType << "] was not found." << std::endl;
			CU_Failure( "Component type was not found." );
			return;
		}

		unregisterComponent( id );
	}

	PassComponentID PassComponentRegister::getNameId( castor::String const & componentType )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&componentType]( Components::value_type const & lookup )
			{
				return lookup.name == componentType;
			} );
		return ( it == m_registered.end() )
			? passcompreg::InvalidId
			: it->id;
	}

	PassComponentPlugin const & PassComponentRegister::getPlugin( PassComponentID componentId )const
	{
		if ( componentId > m_registered.size()
			|| componentId == 0u
			|| !m_registered[componentId - 1ULL].plugin )
		{
			CU_Failure( "Component ID was not found." );
			CU_Exception( "Component ID was not found." );
		}

		return *m_registered[componentId - 1ULL].plugin;
	}

	PassComponentRegister::Component & PassComponentRegister::getNextId()
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin == nullptr;
			} );

		if ( it == m_registered.end() )
		{
			auto id = PassComponentID( m_registered.size() + 1u );
			m_registered.emplace_back( id );
			it = std::next( m_registered.begin()
				, ptrdiff_t( id - 1ULL ) );
		}

		return *it;
	}

	void PassComponentRegister::registerComponent( Component & componentDesc
		, castor::String const & componentType
		, PassComponentPluginUPtr componentPlugin )
	{
		componentPlugin->setId( componentDesc.id );
		componentDesc.name = componentType;
		componentDesc.plugin = castor::move( componentPlugin );

		if ( componentDesc.plugin->getAlphaBlendingFlag() != 0u )
		{
			m_alphaBlendingFlag = componentDesc.plugin->getAlphaBlendingFlag();
		}

		if ( componentDesc.plugin->getAlphaTestFlag() != 0u )
		{
			m_alphaTestFlag = componentDesc.plugin->getAlphaTestFlag();
		}

		if ( componentDesc.plugin->getTransmissionFlag() != 0u )
		{
			m_transmissionFlag = componentDesc.plugin->getTransmissionFlag();
		}

		if ( componentDesc.plugin->getParallaxOcclusionMappingOneFlag() != 0u )
		{
			m_parallaxOcclusionMappingOneFlag = componentDesc.plugin->getParallaxOcclusionMappingOneFlag();
		}

		if ( componentDesc.plugin->getParallaxOcclusionMappingRepeatFlag() != 0u )
		{
			m_parallaxOcclusionMappingRepeatFlag = componentDesc.plugin->getParallaxOcclusionMappingRepeatFlag();
		}

		if ( componentDesc.plugin->getDeferredDiffuseLightingFlag() != 0u )
		{
			m_deferredDiffuseLightingFlag = componentDesc.plugin->getDeferredDiffuseLightingFlag();
		}

		if ( auto shader = componentDesc.plugin->createMaterialShader() )
		{
			m_materialShaders.emplace( componentDesc.id, castor::move( shader ) );

			if ( !m_pauseOrder )
			{
				reorderBuffer();
			}
		}

		castor::AttributeParsers parsers;
		ChannelFillers fillers;
		componentDesc.plugin->createParsers( parsers, fillers );

		for ( auto const & [name, channelFiller] : fillers )
		{
			auto it = m_channelsFillers.try_emplace( name, channelFiller ).first;
			m_channels.try_emplace( it->first, it->second.first );
		}

		castor::StrUInt32Map sections;
		componentDesc.plugin->createSections( sections );

		getEngine()->registerParsers( componentType
			, parsers
			, sections
			, nullptr );
		log::debug << "Registered component ID " << componentDesc.id << " for [" << componentType << "]" << std::endl;
	}

	void PassComponentRegister::unregisterComponent( PassComponentID id )noexcept
	{
		if ( id > 0 && id <= m_registered.size() )
		{
			auto & componentDesc = m_registered[id - 1ULL];
			getEngine()->unregisterParsers( componentDesc.name );

			if ( auto ordIt = std::find( m_bufferOrder.begin(), m_bufferOrder.end(), id );
				ordIt != m_bufferOrder.end() )
			{
				m_bufferOrder.erase( ordIt );
			}

			if ( auto matIt = m_materialShaders.find( id );
				matIt != m_materialShaders.end() )
			{
				m_materialShaders.erase( matIt );

				if ( !m_pauseOrder )
				{
					reorderBuffer();
				}
			}

			auto filIt = m_channelsFillers.begin();

			while ( filIt != m_channelsFillers.end() )
			{
				if ( componentDesc.plugin->getTextureFlags() == filIt->second.first )
				{
					filIt = m_channelsFillers.erase( filIt );
				}
				else
				{
					++filIt;
				}
			}

			log::debug << "Unregistered component " << id << " (" << componentDesc.name << ")" << std::endl;
			componentDesc.name.clear();
			componentDesc.plugin.reset();
		}
	}

	void PassComponentRegister::reorderBuffer()
	{
		m_bufferStride = 0u;
		m_bufferOrder.clear();
		m_bufferShaders.clear();
		m_fillMaterial.clear();
		using NamedChunk = castor::Pair< castor::String, MemChunk >;
		using IdNamedChunk = castor::Pair< PassComponentID, castor::Pair< castor::String, MemChunk > >;
		using Chunks = castor::Map< PassComponentID, NamedChunk >;
		Chunks chunks;

		for ( auto const & [id, shader] : m_materialShaders )
		{
			auto & chunk = shader->getMaterialChunk();
			chunks.try_emplace( id, m_registered[id - 1ULL].name, chunk );
		}

		// float is base unit size.
		// Align on 4 units.
		static uint32_t constexpr alignment = 16u;
		auto constexpr baseOffset = 0u;

		// First put vec4s and 16 bit aligned structs
		castor::Vector< IdNamedChunk > ordered;
		auto it = chunks.begin();
		VkDeviceSize offset = baseOffset;

		while ( it != chunks.end() )
		{
			auto & chunk = *it;

			if ( chunk.second.second.askedSize >= 16u
				&& ( ( chunk.second.second.askedSize % 16u ) == 0u ) )
			{
				chunk.second.second.offset = offset;
				offset += chunk.second.second.askedSize;
				ordered.emplace_back( castor::move( chunk ) );
				it = chunks.erase( it );
			}
			else
			{
				++it;
			}
		}

		// Then put vec3s
		it = chunks.begin();
		VkDeviceSize vec4sIndex = ordered.size();
		VkDeviceSize vec4sOffset = ordered.empty()
			? baseOffset
			: ashes::getAlignedSize( ordered.back().second.second.offset + ordered.back().second.second.askedSize, alignment );
		offset = vec4sOffset;

		while ( it != chunks.end() )
		{
			auto & chunk = *it;

			if ( chunk.second.second.askedSize == 12u )
			{
				chunk.second.second.offset = offset;
				offset += alignment;
				ordered.emplace_back( castor::move( chunk ) );
				it = chunks.erase( it );
			}
			else
			{
				++it;
			}
		}

		// Fill vec3s with floats
		it = chunks.begin();
		auto oit = std::next( ordered.begin(), ptrdiff_t( vec4sIndex ) );
		offset = vec4sOffset + 12u;

		while ( it != chunks.end()
			&& oit != ordered.end() )
		{
			auto & chunk = *it;

			if ( chunk.second.second.askedSize == 4u )
			{
				chunk.second.second.offset = offset;
				offset += alignment;
				++oit;
				oit = ordered.emplace( oit, castor::move( chunk ) );
				it = chunks.erase( it );
				++oit;
			}
			else
			{
				++it;
			}
		}

		// Fill holes with padding
		while ( oit != ordered.end() )
		{
			castor::Pair< PassComponentID, castor::Pair< castor::String, MemChunk > > chunk{};
			chunk.first = passcompreg::InvalidId;
			chunk.second.second.askedSize = 4u;
			chunk.second.second.offset = offset;
			offset += 4u;
			++oit;
			oit = ordered.emplace( oit, castor::move( chunk ) );
			++oit;
		}

		// Carry on with vec2s
		it = chunks.begin();
		VkDeviceSize vec3sOffset = ordered.empty()
			? baseOffset
			: ashes::getAlignedSize( ordered.back().second.second.offset + ordered.back().second.second.askedSize, alignment );
		offset = vec3sOffset;

		while ( it != chunks.end() )
		{
			auto & chunk = *it;

			if ( chunk.second.second.askedSize == 8u )
			{
				chunk.second.second.offset = offset;
				offset += 8u;
				ordered.emplace_back( castor::move( chunk ) );
				it = chunks.erase( it );
			}
			else
			{
				++it;
			}
		}

		// Fill vec2s with floats (0 or 2 only)
		// then finish floats.
		it = chunks.begin();

		while ( it != chunks.end() )
		{
			auto & chunk = *it;

			if ( chunk.second.second.askedSize == 4u )
			{
				chunk.second.second.offset = offset;
				offset += 4u;
				ordered.emplace_back( castor::move( chunk ) );
				it = chunks.erase( it );
			}
			else
			{
				++it;
			}
		}

		// Fill holes with padding
		while ( ( offset % alignment ) != 0u )
		{
			castor::Pair< PassComponentID, castor::Pair< castor::String, MemChunk > > chunk{};
			chunk.first = passcompreg::InvalidId;
			chunk.second.second.askedSize = 4u;
			chunk.second.second.size = 4u;
			chunk.second.second.offset = offset;
			ordered.emplace_back( castor::move( chunk ) );
			offset += 4u;
		}

		// Put the remaining data unpacked
		it = chunks.begin();
		VkDeviceSize floatsOffset = ordered.empty()
			? 0u
			: ashes::getAlignedSize( ordered.back().second.second.offset + ordered.back().second.second.askedSize, alignment );
		offset = floatsOffset;

		while ( it != chunks.end() )
		{
			auto & chunk = *it;
			chunk.second.second.offset = offset;
			offset += ashes::getAlignedSize( chunk.second.second.askedSize, alignment );
			ordered.emplace_back( castor::move( chunk ) );
			it = chunks.erase( it );
		}

		if ( !ordered.empty() )
		{
			auto const & chunk = ordered.back().second.second;
			m_bufferStride = ashes::getAlignedSize( chunk.offset + chunk.askedSize, alignment );
		}

		for ( auto const & [id, pair] : ordered )
		{
			auto matit = m_materialShaders.find( id );

			if ( matit != m_materialShaders.end() )
			{
				m_bufferOrder.push_back( id );
				m_bufferShaders.push_back( matit->second.get() );
				auto shader = m_bufferShaders.back();
				shader->setMaterialChunk( pair.second );
				m_fillMaterial.emplace_back( [shader]( sdw::type::BaseStruct & type
					, sdw::expr::ExprList & inits
					, uint32_t & )
					{
						shader->fillMaterialType( type, inits );
					} );
			}
			else
			{
				m_fillMaterial.emplace_back( []( sdw::type::BaseStruct & type
					, sdw::expr::ExprList & inits
					, uint32_t & padIndex )
					{
						++padIndex;
						type.declMember( "pad" + castor::string::toMbString( padIndex ), ast::type::Kind::eFloat );
						inits.push_back( makeExpr( 0.0_f ) );
					} );
			}
		}
	}

	void PassComponentRegister::fillPassComponentCombine( PassComponentCombine & combine )const
	{
		combine.hasTransmissionFlag = hasAny( combine, m_transmissionFlag );
		combine.hasAlphaTestFlag = hasAny( combine, m_alphaTestFlag );
		combine.hasAlphaBlendingFlag = hasAny( combine, m_alphaBlendingFlag );
		combine.hasParallaxOcclusionMappingOneFlag = hasAny( combine, m_parallaxOcclusionMappingOneFlag );
		combine.hasParallaxOcclusionMappingRepeatFlag = hasAny( combine, m_parallaxOcclusionMappingRepeatFlag );
		combine.hasDeferredDiffuseLightingFlag = hasAny( combine, m_deferredDiffuseLightingFlag );
	}
}
