#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PickableComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileMappingComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ColourComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/HeightComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp"
#include "Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"

CU_ImplementCUSmartPtr( castor3d, PassComponentRegister )

namespace castor3d
{
	namespace passcompreg
	{
		static constexpr PassComponentID InvalidId = PassComponentID( ~PassComponentID{} );

		static bool isValidComponent( PipelineFlags const & flags
			, PassComponentID componentId )
		{
			return flags.components.flags.end() != std::find_if( flags.components.flags.begin()
				, flags.components.flags.end()
				, [&componentId]( PassComponentFlag const & lookup )
				{
					auto lookupComponentId = splitPassComponentFlag( lookup ).first;
					return lookupComponentId == componentId;
				} );
		}

		static bool isValidComponent( TextureCombine const & flags
			, PassComponentID componentId )
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
			, std::vector< shader::PassComponentsShaderPtr > & shaders
			, std::vector< UpdateComponent > & updateComponents )
		{
			if ( isValidComponent( flags, componentId ) )
			{
				if ( component.isComponentNeeded( getTextureFlags( flags ), filter ) )
				{
					if ( auto shader = component.createComponentsShader() )
					{
						shaders.push_back( std::move( shader ) );
					}

					if ( component.updateComponent != nullptr )
					{
						updateComponents.push_back( component.updateComponent );
					}
				}
			}
			else if ( component.updateComponent != nullptr )
			{
				updateComponents.push_back( component.updateComponent );
			}
		}

		static PassComponentID addCombine( PassComponentCombine combine
			, std::vector< PassComponentCombine > & result )
		{
			result.push_back( std::move( combine ) );
			return PassComponentID( result.size() - 1u );
		}
	}

	PassComponentRegister::PassComponentRegister( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
	{
		// 
		// Order is important here, for priority
		// Especially height, opacity and normal which must start
		// the image related components.
		// 

		// Pass base configuration
		registerComponent< PassHeaderComponent >();
		registerComponent< TexturesComponent >();
		registerComponent< TextureCountComponent >();
		registerComponent< BlendComponent >();
		registerComponent< TwoSidedComponent >();
		registerComponent< PickableComponent >();
		registerComponent< UntileMappingComponent >();
		registerComponent< NormalComponent >();
		// Pass shader buffer components
		registerComponent< HeightComponent >();
		registerComponent< OpacityComponent >();
		registerComponent< ColourComponent >();
		registerComponent< EmissiveComponent >();
		registerComponent< TransmissionComponent >();
		registerComponent< SpecularComponent >();
		registerComponent< MetalnessComponent >();
		registerComponent< RoughnessComponent >();
		registerComponent< AlphaTestComponent >();
		registerComponent< ReflectionComponent >();
		registerComponent< RefractionComponent >();
		registerComponent< SubsurfaceScatteringComponent >();
		// Pass shader image components
		registerComponent< HeightMapComponent >();
		registerComponent< NormalMapComponent >();
		registerComponent< OpacityMapComponent >();
		registerComponent< ColourMapComponent >();
		registerComponent< EmissiveMapComponent >();
		registerComponent< OcclusionMapComponent >();
		registerComponent< SpecularMapComponent >();
		registerComponent< MetalnessMapComponent >();
		registerComponent< GlossinessMapComponent >();
		registerComponent< RoughnessMapComponent >();
		registerComponent< TransmittanceMapComponent >();

		m_pauseOrder = false;
		reorderBuffer();
	}

	PassComponentRegister::~PassComponentRegister()
	{
		while ( !m_registered.empty() )
		{
			auto & componentDesc = m_registered.back();

			if ( componentDesc.plugin )
			{
				unregisterComponent( componentDesc.name );
			}

			m_registered.pop_back();
		}
	}

	PassComponentCombine PassComponentRegister::registerPassComponentCombine( Pass const & pass )
	{
		PassComponentCombine result{};

		for ( auto & component : pass.getComponents() )
		{
			addFlags( result, component.second->getPassFlags() );
		}

		result.baseId = registerPassComponentCombine( result );
		return result;
	}

	PassComponentCombineID PassComponentRegister::registerPassComponentCombine( PassComponentCombine combine )
	{
		auto it = std::find_if( m_componentCombines.begin()
			, m_componentCombines.end()
			, [&combine]( PassComponentCombine const & lookup )
			{
				return lookup.baseId == 0
					&& lookup.flags == combine.flags;
			} );

		if ( it == m_componentCombines.end() )
		{
			auto idx = passcompreg::addCombine( std::move( combine ), m_componentCombines );
			it = std::next( m_componentCombines.begin(), idx );
		}

		return PassComponentCombineID( std::distance( m_componentCombines.begin(), it ) + 1 );
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

		return m_componentCombines[id - 1u];
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
			auto & componentDesc = m_registered[id - 1u];
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

	std::vector< shader::PassComponentsShaderPtr > PassComponentRegister::getComponentsShaders( TextureCombine const & combine
		, ComponentModeFlags filter
		, std::vector< UpdateComponent > & updateComponents )const
	{
		std::vector< shader::PassComponentsShaderPtr > result;

		for ( auto & componentDesc : m_registered )
		{
			passcompreg::getComponentShadersT( combine
				, filter
				, componentDesc.id
				, *componentDesc.plugin
				, result
				, updateComponents );
		}

		return result;
	}

	std::vector< shader::PassComponentsShaderPtr > PassComponentRegister::getComponentsShaders( PipelineFlags const & flags
		, ComponentModeFlags filter
		, std::vector< UpdateComponent > & updateComponents )const
	{
		std::vector< shader::PassComponentsShaderPtr > result;

		for ( auto & componentDesc : m_registered )
		{
			passcompreg::getComponentShadersT( flags
				, filter
				, componentDesc.id
				, *componentDesc.plugin
				, result
				, updateComponents );
		}

		return result;
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
		return ( hasAny( flags.components, getAlphaBlendingFlag() )
			|| ( hasAny( flags.components, getAlphaTestFlag() ) && ( flags.alphaFunc != VK_COMPARE_OP_ALWAYS ) ) );
	}

	PassComponentFlag PassComponentRegister::getAlphaBlendingFlag()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->getAlphaBlendingFlag() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getAlphaBlendingFlag();
	}

	PassComponentFlag PassComponentRegister::getAlphaTestFlag()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->getAlphaTestFlag() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getAlphaTestFlag();
	}

	PassComponentFlag PassComponentRegister::getParallaxOcclusionMappingOneFlag()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->getParallaxOcclusionMappingOneFlag() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getParallaxOcclusionMappingOneFlag();
	}

	PassComponentFlag PassComponentRegister::getParallaxOcclusionMappingRepeatFlag()const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin->getParallaxOcclusionMappingRepeatFlag() != 0u;
			} );
		return it == m_registered.end()
			? 0u
			: it->plugin->getParallaxOcclusionMappingRepeatFlag();
	}

	void PassComponentRegister::updateMapComponents( std::vector< TextureFlagConfiguration > const & texConfigs
		, Pass & result )
	{
		std::vector< PassComponentUPtr > components;
		std::map< PassComponentID, Component const * > needed;

		// First gather the needed map components.
		for ( auto & texConfig : texConfigs )
		{
			for ( auto & componentDesc : m_registered )
			{
				if ( componentDesc.plugin->isMapComponent() )
				{
					if ( hasIntersect( texConfig.flag, componentDesc.plugin->getTextureFlags() ) )
					{
						needed.emplace( componentDesc.id, &componentDesc );
					}
				}
			}
		}

		// Then update the pass map components givent the needed ones list.
		for ( auto & componentDesc : m_registered )
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
			result.addComponent( std::move( component ) );
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
		, SceneFileContext & parsingContext )
	{
		for ( auto & channelFiller : m_channelsFillers )
		{
			if ( flags == channelFiller.second.first )
			{
				channelFiller.second.second( parsingContext );
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

	std::map< uint32_t, PassComponentTextureFlag > PassComponentRegister::getTexcoordModifs( PipelineFlags const & flags )const
	{
		auto & textures = flags.textures;
		std::map< uint32_t, PassComponentTextureFlag > result;
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

	std::map< uint32_t, PassComponentTextureFlag > PassComponentRegister::getTexcoordModifs( TextureCombine const & combine )const
	{
		std::map< uint32_t, PassComponentTextureFlag > result;
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
		auto id = getNameId( componentType );

		if ( id != passcompreg::InvalidId )
		{
			log::error << "Pass component type [" << componentType << "] is already registered." << std::endl;
			CU_Failure( "Pass component type is already registered" );
			return id;
		}

		auto & componentDesc = getNextId();
		registerComponent( componentDesc
			, componentType
			, std::move( componentPlugin ) );
		return componentDesc.id;
	}

	void PassComponentRegister::unregisterComponent( castor::String const & componentType )
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
			|| !m_registered[componentId - 1u].plugin )
		{
			CU_Failure( "Component ID was not found." );
			CU_Exception( "Component ID was not found." );
		}

		return *m_registered[componentId - 1u].plugin;
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
			m_registered.push_back( { id } );
			it = std::next( m_registered.begin()
				, ptrdiff_t( id - 1u ) );
		}

		return *it;
	}

	void PassComponentRegister::registerComponent( Component & componentDesc
		, castor::String const & componentType
		, PassComponentPluginUPtr componentPlugin )
	{
		componentPlugin->setId( componentDesc.id );
		componentDesc.name = componentType;
		componentDesc.plugin = std::move( componentPlugin );

		if ( auto shader = componentDesc.plugin->createMaterialShader() )
		{
			m_materialShaders.emplace( componentDesc.id, std::move( shader ) );

			if ( !m_pauseOrder )
			{
				reorderBuffer();
			}
		}

		castor::AttributeParsers parsers;
		ChannelFillers fillers;
		componentDesc.plugin->createParsers( parsers, fillers );

		for ( auto & channelFiller : fillers )
		{
			auto it = m_channelsFillers.emplace( channelFiller ).first;
			m_channels.emplace( it->first, it->second.first );
		}

		castor::StrUInt32Map sections;
		componentDesc.plugin->createSections( sections );

		getEngine()->registerParsers( componentType
			, parsers
			, sections
			, nullptr );
		log::debug << "Registered component ID " << componentDesc.id << " for [" << componentType << "]" << std::endl;
	}

	void PassComponentRegister::unregisterComponent( PassComponentID id )
	{
		if ( id > 0 && id <= m_registered.size() )
		{
			auto & componentDesc = m_registered[id - 1u];
			getEngine()->unregisterParsers( componentDesc.name );

			auto ordIt = std::find( m_bufferOrder.begin(), m_bufferOrder.end(), id );

			if ( ordIt != m_bufferOrder.end() )
			{
				m_bufferOrder.erase( ordIt );
			}

			auto matIt = m_materialShaders.find( id );

			if ( matIt != m_materialShaders.end() )
			{
				m_materialShaders.erase( matIt );

				if ( !m_pauseOrder )
				{
					reorderBuffer();
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
		using Chunks = std::map< PassComponentID, std::pair< std::string, MemChunk > >;
		Chunks chunks;

		for ( auto & shader : m_materialShaders )
		{
			auto chunk = shader.second->getMaterialChunk();
			chunks.emplace( shader.first, std::make_pair( m_registered[shader.first - 1u].name, chunk ) );
		}

		// float is base unit size.
		// Align on 4 units.
		static uint32_t constexpr alignment = 16u;
		auto constexpr baseOffset = 0u;

		// First put vec4s
		std::vector< std::pair< PassComponentID, std::pair< std::string, MemChunk > > > ordered;
		auto it = chunks.begin();
		VkDeviceSize offset = baseOffset;

		while ( it != chunks.end() )
		{
			auto chunk = *it;

			if ( chunk.second.second.askedSize >= 16u
				&& ( ( chunk.second.second.askedSize % 16u ) == 0u ) )
			{
				chunk.second.second.offset = offset;
				offset += alignment;
				ordered.push_back( std::move( chunk ) );
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
			auto chunk = *it;

			if ( chunk.second.second.askedSize == 12u )
			{
				chunk.second.second.offset = offset;
				offset += alignment;
				ordered.push_back( std::move( chunk ) );
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
			auto chunk = *it;

			if ( chunk.second.second.askedSize == 4u )
			{
				chunk.second.second.offset = offset;
				offset += alignment;
				++oit;
				oit = ordered.insert( oit, std::move( chunk ) );
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
			std::pair< PassComponentID, std::pair< std::string, MemChunk > > chunk{};
			chunk.first = passcompreg::InvalidId;
			chunk.second.second.askedSize = 4u;
			chunk.second.second.offset = offset;
			offset += 4u;
			++oit;
			oit = ordered.insert( oit, std::move( chunk ) );
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
			auto chunk = *it;

			if ( chunk.second.second.askedSize == 8u )
			{
				chunk.second.second.offset = offset;
				offset += 8u;
				ordered.push_back( std::move( chunk ) );
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
			auto chunk = *it;

			if ( chunk.second.second.askedSize == 4u )
			{
				chunk.second.second.offset = offset;
				offset += 4u;
				ordered.push_back( std::move( chunk ) );
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
			std::pair< PassComponentID, std::pair< std::string, MemChunk > > chunk{};
			chunk.first = passcompreg::InvalidId;
			chunk.second.second.askedSize = 4u;
			chunk.second.second.offset = offset;
			ordered.push_back( std::move( chunk ) );
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
			auto chunk = *it;
			chunk.second.second.offset = offset;
			offset += ashes::getAlignedSize( chunk.second.second.askedSize, alignment );
			ordered.push_back( std::move( chunk ) );
			it = chunks.erase( it );
		}

		if ( !ordered.empty() )
		{
			auto & chunk = ordered.back().second.second;
			m_bufferStride = ashes::getAlignedSize( chunk.offset + chunk.askedSize, alignment );
		}

		for ( auto & chunkit : ordered )
		{
			auto matit = m_materialShaders.find( chunkit.first );

			if ( matit != m_materialShaders.end() )
			{
				m_bufferOrder.push_back( chunkit.first );
				m_bufferShaders.push_back( matit->second.get() );
				auto shader = m_bufferShaders.back();
				shader->setMaterialChunk( chunkit.second.second );
				m_fillMaterial.push_back( [shader]( sdw::type::BaseStruct & type
					, sdw::expr::ExprList & inits
					, uint32_t & padIndex )
					{
						shader->fillMaterialType( type, inits );
					} );
			}
			else
			{
				m_fillMaterial.push_back( []( sdw::type::BaseStruct & type
					, sdw::expr::ExprList & inits
					, uint32_t & padIndex )
					{
						type.declMember( "pad" + std::to_string( ++padIndex ), ast::type::Kind::eFloat );
						inits.push_back( makeExpr( 0.0_f ) );
					} );
			}
		}
	}
}
