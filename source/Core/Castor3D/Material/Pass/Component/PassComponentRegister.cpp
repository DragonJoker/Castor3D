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

CU_ImplementCUSmartPtr( castor3d, PassComponentRegister )

namespace castor3d
{
	namespace passcompreg
	{
		static constexpr uint32_t InvalidId = ~( 0u );

		static bool isValidComponent( PipelineFlags const & flags
			, uint32_t componentId )
		{
			return flags.components[componentId];
		}

		static bool isValidComponent( TextureFlags const & flags
			, uint32_t componentId )
		{
			return true;
		}

		static TextureFlags const & getTextureFlags( PipelineFlags const & flags )
		{
			return flags.m_texturesFlags;
		}

		static TextureFlags const & getTextureFlags( TextureFlags const & flags )
		{
			return flags;
		}

		template< typename FlagsT >
		void getComponentShadersT( FlagsT const & flags
			, ComponentModeFlags filter
			, uint32_t componentId
			, ComponentData const & component
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
		registerComponent< EmissiveComponent >();
		registerComponent< TransmissionComponent >();
		registerComponent< ColourComponent >();
		registerComponent< SpecularComponent >();
		registerComponent< MetalnessComponent >();
		registerComponent< RoughnessComponent >();
		registerComponent< AlphaTestComponent >();
		registerComponent< ReflectionComponent >();
		registerComponent< RefractionComponent >();
		registerComponent< SubsurfaceScatteringComponent >();
		// Pass shader image components
		registerComponent< HeightMapComponent >();
		registerComponent< OpacityMapComponent >();
		registerComponent< NormalMapComponent >();
		registerComponent< EmissiveMapComponent >();
		registerComponent< OcclusionMapComponent >();
		registerComponent< TransmittanceMapComponent >();
		registerComponent< ColourMapComponent >();
		registerComponent< SpecularMapComponent >();
		registerComponent< MetalnessMapComponent >();
		registerComponent< GlossinessMapComponent >();
		registerComponent< RoughnessMapComponent >();

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
			else if ( componentDesc.data.zeroBuffer )
			{
				componentDesc.data.zeroBuffer( pass
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

	std::vector< shader::PassComponentsShaderPtr > PassComponentRegister::getComponentsShaders( TextureFlags const & texturesFlags
		, ComponentModeFlags filter
		, std::vector< UpdateComponent > & updateComponents )const
	{
		std::vector< shader::PassComponentsShaderPtr > result;

		for ( auto & id : m_ids )
		{
			passcompreg::getComponentShadersT( texturesFlags
				, filter
				, id.first
				, id.second.data
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
				, id.second.data
				, result
				, updateComponents );
		}

		return result;
	}

	uint32_t PassComponentRegister::registerComponent( castor::String const & componentType
		, ParsersFiller createParsers
		, SectionsFiller createSections
		, CreateMaterialShader createMaterialShader
		, ComponentData data )
	{
		auto id = getNameId( componentType );

		if ( id != InvalidIndex )
		{
			log::error << "Pass component type [" << componentType << "] is already registered." << std::endl;
			CU_Failure( "Pass component type is already registered" );
			return id;
		}

		id = getNextId();
		registerComponent( id
			, componentType
			, createParsers
			, createSections
			, createMaterialShader
			, std::move( data ) );
		return id;
	}

	void PassComponentRegister::unregisterComponent( castor::String const & componentType )
	{
		auto id = getNameId( componentType );

		if ( id == InvalidIndex )
		{
			CU_Failure( "" );
			log::error << "Component type [" << componentType << "] was not found." << std::endl;
			return;
		}

		unregisterComponent( id );
	}

	void PassComponentRegister::updateMapComponents( TextureConfiguration const & texConfig
		, Pass & result )
	{
		std::vector< PassComponentUPtr > components;

		for ( auto & idit : m_ids )
		{
			auto & component = idit.second;
			bool hasComponent = result.hasComponent( component.name );

			if ( component.data.needsMapComponent
				&& component.data.createMapComponent
				&& component.data.createMapComponent != &PassComponent::createMapComponent )
			{
				if ( component.data.needsMapComponent( texConfig ) )
				{
					if ( !hasComponent )
					{
						component.data.createMapComponent( result, components );
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

	void PassComponentRegister::fillChannels( TextureFlags const & flags
		, SceneFileContext & parsingContext )
	{
		for ( auto & channelFiller : m_channelsFillers )
		{
			if ( checkFlag( flags, TextureFlags( uint16_t( channelFiller.second.first ) ) ) )
			{
				channelFiller.second.second( parsingContext );
			}
		}
	}

	uint32_t PassComponentRegister::getNameId( castor::String const & componentType )const
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

	uint32_t PassComponentRegister::getNextId()
	{
		static uint32_t result{};
		return result++;
	}

	void PassComponentRegister::registerComponent( uint32_t id
		, castor::String const & componentType
		, ParsersFiller createParsers
		, SectionsFiller createSections
		, CreateMaterialShader createMaterialShader
		, ComponentData data )
	{
		auto & component = m_ids.emplace( id, Component{ componentType, std::move( data ) } ).first->second;
		CU_Require( ( !component.data.zeroBuffer && !createMaterialShader )
			|| ( component.data.zeroBuffer && createMaterialShader ) );

		if ( component.data.zeroBuffer && createMaterialShader )
		{
			if ( auto shader = createMaterialShader() )
			{
				m_materialShaders.emplace( id, std::move( shader ) );

				if ( !m_pauseOrder )
				{
					reorderBuffer();
				}
			}
		}

		castor::AttributeParsers parsers;

		if ( createParsers )
		{
			ChannelFillers fillers;
			createParsers( parsers, fillers );

			for ( auto & channelFiller : fillers )
			{
				auto it = m_channelsFillers.emplace( channelFiller ).first;
				m_channels.emplace( it->first, it->second.first );
			}
		}

		castor::StrUInt32Map sections;

		if ( createSections )
		{
			createSections( sections );
		}

		getEngine()->registerParsers( componentType
			, parsers
			, sections
			, nullptr );
	}

	void PassComponentRegister::unregisterComponent( uint32_t id )
	{
		auto idit = m_ids.find( id );

		if ( idit != m_ids.end() )
		{
			getEngine()->unregisterParsers( idit->second.name );
			m_ids.erase( idit );
		}

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

	void PassComponentRegister::reorderBuffer()
	{
		m_bufferStride = 0u;
		m_bufferOrder.clear();
		m_bufferShaders.clear();
		using Chunks = std::map< uint32_t, std::pair< std::string, MemChunk > >;
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
		std::vector< std::pair< uint32_t, std::pair< std::string, MemChunk > > > ordered;
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
			std::pair< uint32_t, std::pair< std::string, MemChunk > > chunk{};
			chunk.first = uint32_t( ~( 0u ) );
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
			std::pair< uint32_t, std::pair< std::string, MemChunk > > chunk{};
			chunk.first = uint32_t( ~( 0u ) );
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
