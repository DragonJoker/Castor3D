#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"

CU_ImplementSmartPtr( castor3d, SubmeshComponentRegister )

namespace castor3d
{
	namespace smshcompreg
	{
		static constexpr SubmeshComponentID InvalidId = SubmeshComponentID( ~SubmeshComponentID{} );

		static bool isValidComponent( PipelineFlags const & flags
			, SubmeshComponentID componentId )
		{
			return componentId != InvalidId
				&& flags.submesh.flags.end() != flags.submesh.flags.find( componentId );
		}
	}

	SubmeshComponentRegister::SubmeshComponentRegister( Engine & engine )
		: castor::OwnedBy< Engine >{ engine }
	{
		registerComponent< LinesMapping >();
		registerComponent< TriFaceMapping >();
		registerComponent< PositionsComponent >();
		registerComponent< NormalsComponent >();
		registerComponent< TangentsComponent >();
		registerComponent< BitangentsComponent >();
		registerComponent< Texcoords0Component >();
		registerComponent< Texcoords1Component >();
		registerComponent< Texcoords2Component >();
		registerComponent< Texcoords3Component >();
		registerComponent< ColoursComponent >();
		registerComponent< VelocityComponent >();
		registerComponent< SkinComponent >();
		registerComponent< PassMasksComponent >();
		registerComponent< MorphComponent >();
		registerComponent< InstantiationComponent >();
		registerComponent< MeshletComponent >();
		registerComponent< DefaultRenderComponent >();

		addFlags( m_defaultComponents, m_positionFlag );
		addFlags( m_defaultComponents, m_normalFlag );
		addFlags( m_defaultComponents, m_tangentFlag );
		addFlags( m_defaultComponents, m_texcoord0Flag );
		addFlags( m_defaultComponents, *m_renderShaderFlags.begin() );
		registerSubmeshComponentCombine( m_defaultComponents );
	}

	SubmeshComponentRegister::~SubmeshComponentRegister()
	{
		while ( !m_registered.empty() )
		{
			auto & componentDesc = m_registered.back();

			if ( componentDesc.plugin )
			{
				unregisterComponent( componentDesc.id );
			}

			m_registered.pop_back();
		}
	}

	SubmeshComponentCombine SubmeshComponentRegister::registerSubmeshComponentCombine( Submesh const & submesh )
	{
		SubmeshComponentCombine result{};

		for ( auto & component : submesh.getComponents() )
		{
			addFlags( result, component.second->getSubmeshFlags() );
		}

		registerSubmeshComponentCombine( result );
		return result;
	}

	SubmeshComponentCombineID SubmeshComponentRegister::registerSubmeshComponentCombine( SubmeshComponentCombine & combine )
	{
		auto it = std::find_if( m_componentCombines.begin()
			, m_componentCombines.end()
			, [&combine]( SubmeshComponentCombine const & lookup )
			{
				return lookup.flags == combine.flags;
			} );

		if ( it == m_componentCombines.end() )
		{
			m_componentCombines.push_back( combine );
			auto idx = SubmeshComponentCombineID( m_componentCombines.size() - 1u );
			it = std::next( m_componentCombines.begin(), idx );
			it->baseId = idx + 1u;
			fillSubmeshComponentCombine( *it );
		}
		
		fillSubmeshComponentCombine( combine );
		combine.baseId = it->baseId;
		return combine.baseId;
	}

	SubmeshComponentCombineID SubmeshComponentRegister::getSubmeshComponentCombineID( SubmeshComponentCombine const & combine )const
	{
		if ( combine.baseId == 0u )
		{
			auto it = std::find( m_componentCombines.begin()
				, m_componentCombines.end()
				, combine );

			if ( it == m_componentCombines.end() )
			{
				CU_Failure( "Submesh components combination was not registered." );
				CU_Exception( "Submesh components combination was not registered." );
			}

			return SubmeshComponentCombineID( std::distance( m_componentCombines.begin(), it ) + 1 );
		}

		CU_Require( combine.baseId <= m_componentCombines.size() );
		return combine.baseId;
	}

	SubmeshComponentCombine SubmeshComponentRegister::getSubmeshComponentCombine( Submesh const & submesh )const
	{
		return getSubmeshComponentCombine( submesh.getComponentCombineID() );
	}

	SubmeshComponentCombine SubmeshComponentRegister::getSubmeshComponentCombine( SubmeshComponentCombineID id )const
	{
		CU_Require( id <= m_componentCombines.size() );

		if ( id == 0 )
		{
			CU_Failure( "Unexpected invalid pass type." );
			return SubmeshComponentCombine{};
		}

		return m_componentCombines[id - 1u];
	}

	SubmeshData SubmeshComponentRegister::getSubmeshData( SubmeshComponentFlag value )
	{
		if ( value == m_lineIndexFlag
			|| value == m_triangleIndexFlag )
		{
			return SubmeshData::eIndex;
		}

		if ( value == m_positionFlag )
		{
			return SubmeshData::ePositions;
		}

		if ( value == m_normalFlag )
		{
			return SubmeshData::eNormals;
		}

		if ( value == m_tangentFlag )
		{
			return SubmeshData::eTangents;
		}

		if ( value == m_bitangentFlag )
		{
			return SubmeshData::eBitangents;
		}

		if ( value == m_texcoord0Flag )
		{
			return SubmeshData::eTexcoords0;
		}

		if ( value == m_texcoord1Flag )
		{
			return SubmeshData::eTexcoords1;
		}

		if ( value == m_texcoord2Flag )
		{
			return SubmeshData::eTexcoords2;
		}

		if ( value == m_texcoord3Flag )
		{
			return SubmeshData::eTexcoords3;
		}

		if ( value == m_colourFlag )
		{
			return SubmeshData::eColours;
		}

		if ( value == m_skinFlag )
		{
			return SubmeshData::eSkin;
		}

		if ( value == m_passMaskFlag )
		{
			return SubmeshData::ePassMasks;
		}

		if ( value == m_velocityFlag )
		{
			return SubmeshData::eVelocity;
		}

		return SubmeshData::eCount;
	}

	std::vector< shader::SubmeshSurfaceShader * > SubmeshComponentRegister::getSurfaceShaders( PipelineFlags const & flags )const
	{
		std::vector< shader::SubmeshSurfaceShader * > result;

		for ( auto & [componentId, surfaceShader] : m_surfaceShaders )
		{
			if ( smshcompreg::isValidComponent( flags, componentId ) )
			{
				result.push_back( surfaceShader.get() );
			}
		}

		return result;
	}

	void SubmeshComponentRegister::getSubmeshRenderShader( PipelineFlags const & flags
		, ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&flags]( Component const & lookup )
			{
				return hasAny( flags.submesh, lookup.id )
					&& lookup.plugin->getRenderFlag() != 0u;
			} );

		if ( it == m_registered.end() )
		{
			CU_Failure( "Submesh doesn't contain any render shader component" );
			throw std::logic_error{ "Submesh doesn't contain any render shader component" };
		}

		auto rit = m_renderShaders.find( it->id );

		if ( rit == m_renderShaders.end() )
		{
			CU_Failure( "Submesh doesn't contain any render shader component" );
			throw std::logic_error{ "Submesh doesn't contain any render shader component" };
		}

		rit->second->getShaderSource( *getOwner(), flags, componentsMask, builder );
	}

	SubmeshRenderDataPtr SubmeshComponentRegister::createRenderData( SubmeshComponent & component )
	{
		auto it = m_renderShaders.find( component.getId() );

		if ( it == m_renderShaders.end() )
		{
			CU_Failure( "Submesh doesn't contain the wanted render shader component" );
			throw std::logic_error{ "Submesh doesn't contain the wanted render shader component" };
		}

		return it->second->createData( component );
	}

	SubmeshComponentID SubmeshComponentRegister::registerComponent( castor::String const & componentType
		, SubmeshComponentPluginUPtr componentPlugin )
	{
		auto id = getNameId( componentType );

		if ( id != smshcompreg::InvalidId )
		{
			log::error << "Submesh component type [" << componentType << "] is already registered." << std::endl;
			CU_Failure( "Submesh component type is already registered" );
			return id;
		}

		auto & componentDesc = getNextId();
		registerComponent( componentDesc
			, componentType
			, std::move( componentPlugin ) );
		return componentDesc.id;
	}

	void SubmeshComponentRegister::unregisterComponent( castor::String const & componentType )
	{
		auto id = getNameId( componentType );

		if ( id == smshcompreg::InvalidId )
		{
			log::error << "Component type [" << componentType << "] was not found." << std::endl;
			CU_Failure( "Component type was not found." );
			return;
		}

		unregisterComponent( id );
	}

	SubmeshComponentID SubmeshComponentRegister::getNameId( castor::String const & componentType )const
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, [&componentType]( Components::value_type const & lookup )
			{
				return lookup.name == componentType;
			} );
		return ( it == m_registered.end() )
			? smshcompreg::InvalidId
			: it->id;
	}

	SubmeshComponentPlugin const & SubmeshComponentRegister::getPlugin( SubmeshComponentID componentId )const
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

	SubmeshComponentRegister::Component & SubmeshComponentRegister::getNextId()
	{
		auto it = std::find_if( m_registered.begin()
			, m_registered.end()
			, []( Components::value_type const & lookup )
			{
				return lookup.plugin == nullptr;
			} );

		if ( it == m_registered.end() )
		{
			auto id = SubmeshComponentID( m_registered.size() + 1u );
			m_registered.push_back( { id } );
			it = std::next( m_registered.begin()
				, ptrdiff_t( id - 1u ) );
		}

		return *it;
	}

	void SubmeshComponentRegister::registerComponent( Component & componentDesc
		, castor::String const & componentType
		, SubmeshComponentPluginUPtr componentPlugin )
	{
		componentPlugin->setId( componentDesc.id );
		componentDesc.name = componentType;
		componentDesc.plugin = std::move( componentPlugin );

		if ( componentDesc.plugin->getLineIndexFlag() != 0u )
		{
			m_lineIndexFlag = componentDesc.plugin->getLineIndexFlag();
		}

		if ( componentDesc.plugin->getTriangleIndexFlag() != 0u )
		{
			m_triangleIndexFlag = componentDesc.plugin->getTriangleIndexFlag();
		}

		if ( componentDesc.plugin->getPositionFlag() != 0u )
		{
			m_positionFlag = componentDesc.plugin->getPositionFlag();
		}

		if ( componentDesc.plugin->getNormalFlag() != 0u )
		{
			m_normalFlag = componentDesc.plugin->getNormalFlag();
		}

		if ( componentDesc.plugin->getTangentFlag() != 0u )
		{
			m_tangentFlag = componentDesc.plugin->getTangentFlag();
		}

		if ( componentDesc.plugin->getBitangentFlag() != 0u )
		{
			m_bitangentFlag = componentDesc.plugin->getBitangentFlag();
		}

		if ( componentDesc.plugin->getTexcoord0Flag() != 0u )
		{
			m_texcoord0Flag = componentDesc.plugin->getTexcoord0Flag();
		}

		if ( componentDesc.plugin->getTexcoord1Flag() != 0u )
		{
			m_texcoord1Flag = componentDesc.plugin->getTexcoord1Flag();
		}

		if ( componentDesc.plugin->getTexcoord2Flag() != 0u )
		{
			m_texcoord2Flag = componentDesc.plugin->getTexcoord2Flag();
		}

		if ( componentDesc.plugin->getTexcoord3Flag() != 0u )
		{
			m_texcoord3Flag = componentDesc.plugin->getTexcoord3Flag();
		}

		if ( componentDesc.plugin->getColourFlag() != 0u )
		{
			m_colourFlag = componentDesc.plugin->getColourFlag();
		}

		if ( componentDesc.plugin->getSkinFlag() != 0u )
		{
			m_skinFlag = componentDesc.plugin->getSkinFlag();
		}

		if ( componentDesc.plugin->getMorphFlag() != 0u )
		{
			m_morphFlag = componentDesc.plugin->getMorphFlag();
		}

		if ( componentDesc.plugin->getPassMaskFlag() != 0u )
		{
			m_passMaskFlag = componentDesc.plugin->getPassMaskFlag();
		}

		if ( componentDesc.plugin->getVelocityFlag() != 0u )
		{
			m_velocityFlag = componentDesc.plugin->getVelocityFlag();
		}

		if ( auto shader = componentDesc.plugin->createSurfaceShader() )
		{
			m_surfaceShaders.emplace( componentDesc.id, std::move( shader ) );
		}

		if ( auto shader = componentDesc.plugin->createRenderShader() )
		{
			m_renderShaders.emplace( componentDesc.id, std::move( shader ) );
			m_renderShaderFlags.emplace_back( componentDesc.plugin->getComponentFlags() );
		}

		castor::AttributeParsers parsers;
		componentDesc.plugin->createParsers( parsers );

		castor::StrUInt32Map sections;
		componentDesc.plugin->createSections( sections );

		getEngine()->registerParsers( componentType
			, parsers
			, sections
			, componentDesc.plugin->createParserContext );

		log::debug << "Registered component ID " << componentDesc.id << " for [" << componentType << "]" << std::endl;
	}

	void SubmeshComponentRegister::unregisterComponent( SubmeshComponentID id )
	{
		if ( id > 0 && id <= m_registered.size() )
		{
			auto & componentDesc = m_registered[id - 1u];
			getEngine()->unregisterParsers( componentDesc.name );
			auto sit = m_surfaceShaders.find( id );

			if ( sit != m_surfaceShaders.end() )
			{
				m_surfaceShaders.erase( sit );
			}

			auto rit = m_renderShaders.find( id );

			if ( rit != m_renderShaders.end() )
			{
				m_renderShaders.erase( rit );
			}

			auto flag = componentDesc.plugin->getComponentFlags();
			auto fit = std::find( m_renderShaderFlags.begin()
				, m_renderShaderFlags.end()
				, flag );

			if ( fit != m_renderShaderFlags.end() )
			{
				m_renderShaderFlags.erase( fit );
			}

			log::debug << "Unregistered component " << id << " (" << componentDesc.name << ")" << std::endl;
			componentDesc.name.clear();
			componentDesc.plugin.reset();
			componentDesc.id = smshcompreg::InvalidId;
		}
	}

	void SubmeshComponentRegister::fillSubmeshComponentCombine( SubmeshComponentCombine & combine )
	{
		combine.hasLineIndexFlag = hasAny( combine, m_lineIndexFlag );
		combine.hasTriangleIndexFlag = hasAny( combine, m_triangleIndexFlag );
		combine.hasPositionFlag = hasAny( combine, m_positionFlag );
		combine.hasNormalFlag = hasAny( combine, m_normalFlag );
		combine.hasTangentFlag = hasAny( combine, m_tangentFlag );
		combine.hasBitangentFlag = hasAny( combine, m_bitangentFlag );
		combine.hasTexcoord0Flag = hasAny( combine, m_texcoord0Flag );
		combine.hasTexcoord1Flag = hasAny( combine, m_texcoord1Flag );
		combine.hasTexcoord2Flag = hasAny( combine, m_texcoord2Flag );
		combine.hasTexcoord3Flag = hasAny( combine, m_texcoord3Flag );
		combine.hasColourFlag = hasAny( combine, m_colourFlag );
		combine.hasSkinFlag = hasAny( combine, m_skinFlag );
		combine.hasMorphFlag = hasAny( combine, m_morphFlag );
		combine.hasPassMaskFlag = hasAny( combine, m_passMaskFlag );
		combine.hasVelocityFlag = hasAny( combine, m_velocityFlag );
		combine.hasRenderFlag = hasAny( combine, m_renderShaderFlags );
	}
}
