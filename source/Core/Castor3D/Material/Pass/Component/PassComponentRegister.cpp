#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/Base/BlendComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/NormalComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp"
#include "Castor3D/Material/Pass/Component/Base/UntileComponent.hpp"
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
			return flags.components.end() != flags.components.find( componentId );
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

		static PassComponentIDSet getComponents( Pass const & pass )
		{
			PassComponentIDSet components;

			for ( auto & component : pass.getComponents() )
			{
				components.emplace( component.first );
			}

			return components;
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
		registerComponent< UntileComponent >();
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
		while ( !m_ids.empty() )
		{
			auto it = m_ids.begin();
			unregisterComponent( it->second.name );
		}
	}

	PassComponentsTypeID PassComponentRegister::registerPassType( Pass const & pass )
	{
		auto components = passcompreg::getComponents( pass );
		auto it = std::find( m_passTypes.begin()
			, m_passTypes.end()
			, components );

		if ( it == m_passTypes.end() )
		{
			m_passTypes.push_back( std::move( components ) );
			it = std::next( m_passTypes.begin(), ptrdiff_t( m_passTypes.size() - 1u ) );
		}

		return PassComponentsTypeID( std::distance( m_passTypes.begin(), it ) + 1 );
	}

	PassComponentsTypeID PassComponentRegister::getPassComponentsType( Pass const & pass )const
	{
		return getPassComponentsType( passcompreg::getComponents( pass ) );
	}

	PassComponentsTypeID PassComponentRegister::getPassComponentsType( PassComponentIDSet const & components )const
	{
		auto it = std::find( m_passTypes.begin()
			, m_passTypes.end()
			, components );
		CU_Require( it != m_passTypes.end() );
		return it == m_passTypes.end()
			? 0u
			: PassComponentsTypeID( std::distance( m_passTypes.begin(), it ) + 1 );
	}

	TextureCombineID PassComponentRegister::getTextureCombineType( Pass const & pass )const
	{
		return getOwner()->getTextureUnitCache().getTextureCombineType( pass );
	}

	TextureCombineID PassComponentRegister::getTextureCombineType( TextureCombine const & combine )const
	{
		return getOwner()->getTextureUnitCache().getTextureCombineType( combine );
	}

	PassComponentIDSet PassComponentRegister::getPassComponents( Pass const & pass )const
	{
		return getPassComponents( pass.getComponentsTypeID() );
	}

	PassComponentIDSet PassComponentRegister::getPassComponents( PassComponentsTypeID passType )const
	{
		CU_Require( passType <= m_passTypes.size() );

		if ( passType == 0 )
		{
			CU_Failure( "Unexpected invalid pass type." );
			return PassComponentIDSet{};
		}

		return m_passTypes[passType - 1u];
	}

	TextureCombine PassComponentRegister::getTextureCombine( Pass const & pass )const
	{
		return getTextureCombine( pass.getTextureCombineType() );
	}

	TextureCombine PassComponentRegister::getTextureCombine( TextureCombineID combineType )const
	{
		return getOwner()->getTextureUnitCache().getTextureCombine( combineType );
	}

	void PassComponentRegister::fillBuffer( Pass const & pass
		, PassBuffer & buffer )const
	{
		for ( auto & id : m_bufferOrder )
		{
			auto & componentDesc = m_ids.find( id )->second;

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

		for ( auto & id : m_ids )
		{
			passcompreg::getComponentShadersT( combine
				, filter
				, id.first
				, *id.second.plugin
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

		for ( auto & id : m_ids )
		{
			passcompreg::getComponentShadersT( flags
				, filter
				, id.first
				, *id.second.plugin
				, result
				, updateComponents );
		}

		return result;
	}

	void PassComponentRegister::updateMapComponents( std::vector< TextureFlagConfiguration > const & texConfigs
		, Pass & result )
	{
		std::vector< PassComponentUPtr > components;
		std::map< PassComponentID, Component const * > needed;

		// First gather the needed map components.
		for ( auto & texConfig : texConfigs )
		{
			for ( auto & idit : m_ids )
			{
				auto & component = idit.second;

				if ( component.plugin->isMapComponent() )
				{
					if ( hasIntersect( texConfig.flag, component.plugin->getTextureFlags() ) )
					{
						needed.emplace( idit.first, &idit.second );
					}
				}
			}
		}

		// Then update the pass map components givent the needed ones list.
		for ( auto & idit : m_ids )
		{
			auto & component = idit.second;

			if ( component.plugin->isMapComponent() )
			{
				auto needit = needed.find( idit.first );
				bool hasComponent = result.hasComponent( component.name );

				if ( needit != needed.end() )
				{
					if ( !hasComponent )
					{
						component.plugin->createMapComponent( result, components );
					}
				}
				else if ( hasComponent )
				{
					result.removeComponent( component.name );
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

		for ( auto & idit : m_ids )
		{
			result = result
				&& idit.second.plugin->writeTextureConfig( configuration, tabs, file );
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

		for ( auto & id : m_ids )
		{
			id.second.plugin->filterTextureFlags( filter, result );
		}

		result.configCount = result.flags.empty()
			? 0u
			: result.configCount;
		return result;
	}

	PassComponentTextureFlag PassComponentRegister::getColourFlags()const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, []( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& lookup.second.plugin->getColourFlags() != 0u;
			} );
		return it == m_ids.end()
			? 0u
			: it->second.plugin->getColourFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getOpacityFlags()const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, []( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& lookup.second.plugin->getOpacityFlags() != 0u;
			} );
		return it == m_ids.end()
			? 0u
			: it->second.plugin->getOpacityFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getNormalFlags()const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, []( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& lookup.second.plugin->getNormalFlags() != 0u;
			} );
		return it == m_ids.end()
			? 0u
			: it->second.plugin->getNormalFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getHeightFlags()const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, []( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& lookup.second.plugin->getHeightFlags() != 0u;
			} );
		return it == m_ids.end()
			? 0u
			: it->second.plugin->getHeightFlags();
	}

	PassComponentTextureFlag PassComponentRegister::getOcclusionFlags()const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, []( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& lookup.second.plugin->getOcclusionFlags() != 0u;
			} );
		return it == m_ids.end()
			? 0u
			: it->second.plugin->getOcclusionFlags();
	}

	void PassComponentRegister::fillTextureConfiguration( PassComponentTextureFlag const & flags
		, TextureConfiguration & result )const
	{
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, [&flags]( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& hasIntersect( flags, lookup.second.plugin->getTextureFlags() );
			} );

		if ( it != m_ids.end() )
		{
			it->second.plugin->fillTextureConfiguration( result );
		}
	}

	bool PassComponentRegister::hasTexcoordModif( PassComponentTextureFlag const & flag
		, PipelineFlags const * flags )const
	{
		return m_ids.end() != std::find_if( m_ids.begin()
			, m_ids.end()
			, [&flag, flags]( ComponentIds::value_type const & lookup )
			{
				return lookup.second.plugin->isMapComponent()
					&& hasIntersect( flag, lookup.second.plugin->getTextureFlags() )
					&& lookup.second.plugin->hasTexcoordModif( flags );
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

		id = getNextId();
		registerComponent( id
			, componentType
			, std::move( componentPlugin ) );
		return id;
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
		auto it = std::find_if( m_ids.begin()
			, m_ids.end()
			, [&componentType]( ComponentIds::value_type const & lookup )
			{
				return lookup.second.name == componentType;
			} );
		return ( it == m_ids.end() )
			? passcompreg::InvalidId
			: it->first;
	}
	
	PassComponentsBitset PassComponentRegister::getPassComponentsBitset( Pass const * pass )const
	{
		PassComponentsBitset result{ m_ids.size() };

		if ( pass )
		{
			for ( auto & id : m_ids )
			{
				result[id.first] = pass->hasComponent( id.second.name );
			}
		}

		return result;
	}

	PassComponentPlugin const & PassComponentRegister::getPlugin( PassComponentID componentId )const
	{
		auto it = m_ids.find( componentId );

		if ( it == m_ids.end() )
		{
			CU_Failure( "Component ID was not found." );
			CU_Exception( "Component ID was not found." );
		}

		return *it->second.plugin;
	}

	PassComponentID PassComponentRegister::getNextId()
	{
		static PassComponentID result{};
		return result++;
	}

	void PassComponentRegister::registerComponent( PassComponentID id
		, castor::String const & componentType
		, PassComponentPluginUPtr componentPlugin )
	{
		componentPlugin->setId( id );
		auto & component = m_ids.emplace( id, Component{ componentType, std::move( componentPlugin ) } ).first->second;

		if ( auto shader = component.plugin->createMaterialShader() )
		{
			m_materialShaders.emplace( id, std::move( shader ) );

			if ( !m_pauseOrder )
			{
				reorderBuffer();
			}
		}

		castor::AttributeParsers parsers;
		ChannelFillers fillers;
		component.plugin->createParsers( parsers, fillers );

		for ( auto & channelFiller : fillers )
		{
			auto it = m_channelsFillers.emplace( channelFiller ).first;
			m_channels.emplace( it->first, it->second.first );
		}

		castor::StrUInt32Map sections;
		component.plugin->createSections( sections );

		getEngine()->registerParsers( componentType
			, parsers
			, sections
			, nullptr );

		log::debug << "Registered component ID " << id << " for [" << componentType << "]" << std::endl;
	}

	void PassComponentRegister::unregisterComponent( PassComponentID id )
	{
		auto idit = m_ids.find( id );

		if ( idit != m_ids.end() )
		{
			log::debug << "Unregistered component " << id << " (" << idit->second.name << ")" << std::endl;
			getEngine()->unregisterParsers( idit->second.name );
			m_ids.erase( idit );

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
			chunks.emplace( shader.first, std::make_pair( m_ids[shader.first].name, chunk ) );
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
