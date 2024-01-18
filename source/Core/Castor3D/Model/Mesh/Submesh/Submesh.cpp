#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, Submesh )

namespace castor3d
{
	//*********************************************************************************************

	namespace smsh
	{
		static size_t hash( Submesh const & submesh
			, Geometry const & geometry
			, Pass const & pass
			, PipelineFlags const & flags )
		{
			size_t result = geometry.getHash( pass, submesh );
			result = castor::hashCombine( result, flags.m_shaderFlags.value() );
			result = castor::hashCombine( result, flags.m_programFlags.value() );
			result = castor::hashCombine( result, flags.submesh.baseId );
			return result;
		}

		static bool fix( castor::Point3f & value
			, castor::Point3f const & defaultValue )
		{
			bool result = false;

			if ( std::isnan( value->x ) )
			{
				value->x = defaultValue->x;
				result = true;
			}

			if ( std::isnan( value->y ) )
			{
				value->z = defaultValue->y;
				result = true;
			}

			if ( std::isnan( value->z ) )
			{
				value->z = defaultValue->z;
				result = true;
			}

			return result;
		}

		static bool fixNml( castor::Point3f & value )
		{
			static castor::Point3f const defaultValue{ 0.0f, 1.0f, 0.0f };
			auto result = fix( value, defaultValue );

			if ( castor::point::length( value ) < std::numeric_limits< float >::epsilon() )
			{
				value = defaultValue;
				result = true;
			}

			return result;
		}

		static bool fixPos( castor::Point3f & value )
		{
			static castor::Point3f const defaultValue{ 0.0f, 0.0f, 0.0f };
			return fix( value, defaultValue );
		}

		static bool fixTex( castor::Point3f & value )
		{
			static castor::Point3f const defaultValue{ 0.0f, 0.0f, 0.0f };
			return fix( value, defaultValue );
		}

		template< typename ComponentT >
		static uint32_t getComponentCount( Submesh const & submesh )
		{
			uint32_t result{};

			if ( auto comp = submesh.getComponent< ComponentT >() )
			{
				if ( auto compData = comp->getBaseData() )
				{
					result = uint32_t( static_cast< typename ComponentT::ComponentData * >( compData )->getData().size() );
				}
			}

			return result;
		}

		template< typename ComponentT, typename DataT >
		void addComponentData( Submesh & submesh
			, DataT const & data )
		{
			if ( auto comp = submesh.getComponent< ComponentT >() )
			{
				if ( auto compData = comp->getBaseData() )
				{
					static_cast< typename ComponentT::ComponentData * >( compData )->getData().push_back( data );
				}
			}
		}

		template< typename ComponentT >
		void reserveComponentData( Submesh & submesh
			, uint32_t size )
		{
			if ( auto comp = submesh.getComponent< ComponentT >() )
			{
				if ( auto compData = comp->getBaseData() )
				{
					static_cast< typename ComponentT::ComponentData * >( compData )->getData().reserve( size );
				}
			}
		}
	}

	//*********************************************************************************************

	Submesh::Submesh( Mesh & mesh
		, uint32_t id )
		: OwnedBy< Mesh >{ mesh }
		, m_id{ id }
		, m_defaultMaterial{ mesh.getScene()->getEngine()->getMaterialCache().getDefaultMaterial() }
	{
		createComponent< InstantiationComponent >( 2u );
	}

	Submesh::~Submesh()
	{
		CU_Assert( !m_initialised, "Did you forget to call Submesh::cleanup ?" );
	}

	void Submesh::initialise( RenderDevice const & device )
	{
		if ( !m_generated )
		{
			if ( !m_sourceBufferOffset
				|| getPointsCount() != m_sourceBufferOffset.getCount< castor::Point4f >( SubmeshData::ePositions ) )
			{
				for ( auto & finalBufferOffset : m_finalBufferOffsets )
				{
					if ( finalBufferOffset.second )
					{
						device.geometryPools->putBuffer( finalBufferOffset.second );
						finalBufferOffset.second = {};
					}
				}

				if ( m_sourceBufferOffset )
				{
					device.geometryPools->putBuffer( m_sourceBufferOffset );
					m_sourceBufferOffset = {};
				}

				VkDeviceSize indexCount = 0u;

				if ( m_indexMapping )
				{
					indexCount = VkDeviceSize( m_indexMapping->getCount() ) * m_indexMapping->getComponentsCount();
				}

				if ( isDynamic()
					&& !hasComponent( BaseDataComponentT< SubmeshData::eVelocity >::TypeName ) )
				{
					createComponent< BaseDataComponentT< SubmeshData::eVelocity > >();
				}

				auto & components = getOwner()->getOwner()->getSubmeshComponentsRegister();
				m_componentCombine = components.registerSubmeshComponentCombine( *this );

				auto combine = m_componentCombine;
				remFlags( combine, components.getVelocityFlag() );
				combine.hasVelocityFlag = false;
				components.registerSubmeshComponentCombine( combine );
				m_sourceBufferOffset = device.geometryPools->getBuffer( getPointsCount()
					, indexCount
					, getMeshletsCount()
					, combine );

				if ( !m_sourceBufferOffset.hasData( SubmeshData::ePositions )
					|| !m_sourceBufferOffset.getBufferChunk( SubmeshData::ePositions ).buffer )
				{
					CU_Failure( "No source data available for submesh" );
					CU_Exception( "No source data available for submesh" );
				}

				if ( isDynamic() )
				{
					ashes::BufferBase const * indexBuffer{};

					if ( m_indexMapping )
					{
						indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshData::eIndex );
					}

					combine = m_componentCombine;
					remFlags( combine, components.getSkinFlag() );
					combine.hasSkinFlag = false;
					components.registerSubmeshComponentCombine( combine );

					for ( auto & finalBufferOffset : m_finalBufferOffsets )
					{
						finalBufferOffset.second = device.geometryPools->getBuffer( getPointsCount()
							, indexBuffer
							, combine );

						if ( !finalBufferOffset.second.hasData( SubmeshData::ePositions )
							|| !finalBufferOffset.second.getBufferChunk( SubmeshData::ePositions ).buffer )
						{
							CU_Failure( "No final data available for submesh" );
							CU_Exception( "No final data available for submesh" );
						}
					}
				}
			}

			m_generated = true;
		}

		if ( !m_initialised )
		{
			m_initialised = true;

			for ( auto & component : m_components )
			{
				if ( m_initialised )
				{
					if ( auto data = component.second->getBaseData() )
					{
						m_initialised = data->initialise( device );
					}
				}

				if ( m_initialised )
				{
					if ( auto data = component.second->getRenderData() )
					{
						m_initialised = data->initialise( device );
					}
				}
			}

			m_dirty = !m_initialised;
		}
	}

	void Submesh::cleanup( RenderDevice const & device )
	{
		m_initialised = false;

		for ( auto & component : m_components )
		{
			if ( auto data = component.second->getRenderData() )
			{
				data->cleanup( device );
			}

			if ( auto data = component.second->getBaseData() )
			{
				data->cleanup( device );
			}
		}

		for ( auto & finalBufferOffset : m_finalBufferOffsets )
		{
			device.geometryPools->putBuffer( finalBufferOffset.second );
			finalBufferOffset.second = {};
		}

		if ( m_sourceBufferOffset )
		{
			device.geometryPools->putBuffer( m_sourceBufferOffset );
		}
	}

	void Submesh::upload( UploadData & uploader )
	{
		m_dirty = false;

		for ( auto & component : m_components )
		{
			if ( auto data = component.second->getBaseData() )
			{
				data->upload( uploader );
			}
		}
	}

	void Submesh::update( CpuUpdater & updater )
	{
		for ( auto & component : m_components )
		{
			if ( auto data = component.second->getRenderData() )
			{
				data->update( updater );
			}
		}
	}

	crg::FramePassArray Submesh::record( crg::ResourcesCache & resources
		, crg::FrameGraph & graph
		, crg::FramePassArray previousPasses )
	{
		auto & device = *getParent().getOwner()->getRenderDevice();

		for ( auto & component : m_components )
		{
			if( auto data = component.second->getRenderData() )
			{
				previousPasses = data->record( device, resources, graph, std::move( previousPasses ) );
			}
		}

		return previousPasses;
	}

	void Submesh::registerDependencies( crg::FramePass & pass )const
	{
		for ( auto & component : m_components )
		{
			if ( auto data = component.second->getRenderData() )
			{
				data->registerDependencies( pass );
			}
		}
	}

	void Submesh::accept( ConfigurationVisitorBase & vis )
	{
		castor::StringArray topologies;
		topologies.push_back( cuT( "Point List" ) );
		topologies.push_back( cuT( "Line List" ) );
		topologies.push_back( cuT( "Line Strip" ) );
		topologies.push_back( cuT( "Triangle List" ) );
		topologies.push_back( cuT( "Triangle Strip" ) );
		topologies.push_back( cuT( "Triangle Fan" ) );
		topologies.push_back( cuT( "Line List With Adjacency" ) );
		topologies.push_back( cuT( "Line Strip With Adjacency" ) );
		topologies.push_back( cuT( "Triangle List With Adjacency" ) );
		topologies.push_back( cuT( "Triangle Strip With Adjacency" ) );
		topologies.push_back( cuT( "Patch List" ) );
		vis.visit< VkPrimitiveTopology >( cuT( "Topology" ), m_topology, topologies
			, [this]( int oldV, int newV )
			{
				m_topology = VkPrimitiveTopology( newV );
			} );

		for ( auto & component : m_components )
		{
			component.second->accept( vis );
		}
	}

	void Submesh::computeContainers()
	{
		if ( !m_dirty )
		{
			return;
		}

		auto positions = getComponent< PositionsComponent >();

		if ( positions && getPointsCount() )
		{
			auto & points = positions->getData().getData();
			castor::Point3f min{ points[0] };
			castor::Point3f max{ points[0] };
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				castor::Point3f cur{ points[i] };
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			m_box.load( min, max );
			m_sphere.load( m_box );

			if ( m_needsNormalsCompute )
			{
				computeNormals();
			}
		}
	}

	void Submesh::updateContainers( castor::BoundingBox const & boundingBox )
	{
		m_box = boundingBox;
		m_sphere.load( m_box );
	}

	uint32_t Submesh::getFaceCount()const
	{
		uint32_t result = 0u;
		
		if ( m_indexMapping )
		{
			result = m_indexMapping->getCount();
		}
		else
		{
			result = getPointsCount();

			switch ( getTopology() )
			{
			case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
				break;

			case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
				result /= 2;
				break;

			case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
				result -= 1u;
				break;

			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
				result /= 3u;
				break;

			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
				result -= 2u;
				break;

			default:
				break;
			}
		}

		return result;
	}

	uint32_t Submesh::getPointsCount()const
	{
		return std::max( { smsh::getComponentCount< PositionsComponent >( *this )
			, smsh::getComponentCount< NormalsComponent >( *this )
			, smsh::getComponentCount< TangentsComponent >( *this )
			, smsh::getComponentCount< BitangentsComponent >( *this )
			, smsh::getComponentCount< Texcoords0Component >( *this )
			, smsh::getComponentCount< Texcoords1Component >( *this )
			, smsh::getComponentCount< Texcoords2Component >( *this )
			, smsh::getComponentCount< Texcoords3Component >( *this )
			, smsh::getComponentCount< ColoursComponent >( *this )
			, uint32_t( m_sourceBufferOffset ? m_sourceBufferOffset.getCount< castor::Point4f >( SubmeshData::ePositions ) : 0u ) } );
	}

	int Submesh::isInMyPoints( castor::Point3f const & vertex
		, double precision )
	{
		int result = -1;
		auto positions = getComponent< PositionsComponent >();

		if ( positions )
		{
			int index = 0;
			auto & points = positions->getData().getData();

			for ( auto it = points.begin(); it != points.end() && result == -1; ++it )
			{
				if ( castor::point::distanceSquared( vertex, castor::Point3f{ *it } ) < precision )
				{
					result = index;
				}

				index++;
			}
		}

		return result;
	}

	InterleavedVertex Submesh::addPoint( float x, float y, float z )
	{
		InterleavedVertex result;
		result.pos = castor::Point3f{ x, y, z };
		addPoint( result );
		return result;
	}

	InterleavedVertex Submesh::addPoint( castor::Point3f const & value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	InterleavedVertex Submesh::addPoint( float * value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	void Submesh::addPoint( InterleavedVertex const & vertex )
	{
		auto point = vertex;
		m_needsNormalsCompute = smsh::fixNml( point.nml );
		m_needsNormalsCompute = smsh::fixPos( point.pos ) || m_needsNormalsCompute;
		m_needsNormalsCompute = smsh::fixTex( point.tex ) || m_needsNormalsCompute;
		smsh::addComponentData< PositionsComponent >( *this, point.pos );
		smsh::addComponentData< NormalsComponent >( *this, point.nml );
		smsh::addComponentData< TangentsComponent >( *this, point.tan );
		smsh::addComponentData< Texcoords0Component >( *this, point.tex );
	}

	void Submesh::addPoints( InterleavedVertex const * const begin
		, InterleavedVertex const * const end )
	{
		auto size = getPointsCount() + uint32_t( std::distance( begin, end ) );
		smsh::reserveComponentData< PositionsComponent >( *this, size );
		smsh::reserveComponentData< NormalsComponent >( *this, size );
		smsh::reserveComponentData< TangentsComponent >( *this, size );
		smsh::reserveComponentData< Texcoords0Component >( *this, size );

		for ( auto & point : castor::makeArrayView( begin, end ) )
		{
			addPoint( point );
		}
	}

	void Submesh::computeNormals( bool reverted )
	{
		if ( m_indexMapping )
		{
			m_indexMapping->computeNormals( reverted );
			m_indexMapping->computeTangents();
			m_needsNormalsCompute = false;
		}
	}

	ProgramFlags Submesh::getProgramFlags( Pass const & pass )const noexcept
	{
		ProgramFlags result{};

		for ( auto & component : m_components )
		{
			result |= component.second->getProgramFlags( pass );
		}

		return result;
	}

	MorphFlags Submesh::getMorphFlags()const
	{
		MorphFlags result{};

		if ( auto morph = getComponent< MorphComponent >() )
		{
			result = morph->getData().getMorphFlags();
		}

		return result;
	}

	GeometryBuffers const & Submesh::getGeometryBuffers( Geometry const & geometry
		, Pass const & pass
		, PipelineFlags const & flags )const
	{
		auto key = smsh::hash( *this, geometry, pass, flags );
		auto ires = m_geometryBuffers.emplace( key, GeometryBuffers{} );
		auto & bufferOffsets = getFinalBufferOffsets( geometry, pass );

		if ( ires.second )
		{
			ashes::BufferCRefArray buffers;
			ashes::UInt64Array offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			uint32_t currentBinding = 0u;
			uint32_t currentLocation = 0u;

			for ( auto & component : m_components )
			{
				if ( auto data = component.second->getBaseData() )
				{
					data->gather( flags
						, pass
						, bufferOffsets
						, buffers
						, offsets
						, layouts
						, currentBinding
						, currentLocation );
					CU_Require( layouts.size() == buffers.size() );
					CU_Require( offsets.size() == buffers.size() );
				}
			}

			auto & result = ires.first->second;
			result.indexOffset = getSourceBufferOffsets().getBufferChunk( SubmeshData::eIndex );
			result.layouts = layouts;
			result.buffers = buffers;
			result.offsets = offsets;
		}

		return ires.first->second;
	}

	void Submesh::enableSceneUpdate( bool updateScene )
	{
		m_disableSceneUpdate = false;
	}

	void Submesh::setBaseData( SubmeshData submeshData, castor::Point3fArray data )
	{
		switch ( submeshData )
		{
		case castor3d::SubmeshData::ePositions:
			getPositions() = std::move( data );
			break;
		case castor3d::SubmeshData::eNormals:
			getNormals() = std::move( data );
			break;
		case castor3d::SubmeshData::eTexcoords0:
			getTexcoords0() = std::move( data );
			break;
		case castor3d::SubmeshData::eTexcoords1:
			getTexcoords1() = std::move( data );
			break;
		case castor3d::SubmeshData::eTexcoords2:
			getTexcoords2() = std::move( data );
			break;
		case castor3d::SubmeshData::eTexcoords3:
			getTexcoords3() = std::move( data );
			break;
		case castor3d::SubmeshData::eColours:
			getColours() = std::move( data );
			break;
		case castor3d::SubmeshData::eBitangents:
			getBitangents() = std::move( data );
			break;
		case castor3d::SubmeshData::eTangents:
			CU_Failure( "setBaseData: Can't set tangent data this way, use the Point4f variant" );
			break;
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "setBaseData: Can't set index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case castor3d::SubmeshData::ePassMasks:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "setBaseData: Can't set velocity data this way" );
			break;
		case castor3d::SubmeshData::eMeshlets:
			CU_Failure( "setBaseData: Can't set meshlets data this way" );
			break;
		default:
			CU_Failure( "setBaseData: Unsupported SubmeshData type" );
			break;
		}
	}

	void Submesh::setBaseData( SubmeshData submeshData, castor::Point4fArray data )
	{
		switch ( submeshData )
		{
		case castor3d::SubmeshData::ePositions:
			CU_Failure( "setBaseData: Can't set positions data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eNormals:
			CU_Failure( "setBaseData: Can't set normals data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eTangents:
			getTangents() = std::move( data );
			break;
		case castor3d::SubmeshData::eBitangents:
			CU_Failure( "setBaseData: Can't set bitangents data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eTexcoords0:
			CU_Failure( "setBaseData: Can't set texcoords0 data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eTexcoords1:
			CU_Failure( "setBaseData: Can't set texcoords1 data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eTexcoords2:
			CU_Failure( "setBaseData: Can't set texcoords2 data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eTexcoords3:
			CU_Failure( "setBaseData: Can't set texcoords3 data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eColours:
			CU_Failure( "setBaseData: Can't set colours data this way, use the Point3f variant" );
			break;
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "setBaseData: Can't set index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case castor3d::SubmeshData::ePassMasks:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "setBaseData: Can't set velocity data this way" );
			break;
		case castor3d::SubmeshData::eMeshlets:
			CU_Failure( "setBaseData: Can't set meshlets data this way" );
			break;
		default:
			CU_Failure( "setBaseData: Unsupported SubmeshData type" );
			break;
		}
	}

	void Submesh::addComponent( SubmeshComponentUPtr component )
	{
		if ( component->getPlugin().getRenderFlag()
			&& m_render != component.get() )
		{
			if ( m_render )
			{
				m_components.erase( m_render->getId() );
			}

			m_render = component.get();
			m_render->initialiseRenderData();
		}
		else if ( component->getPlugin().getIndexFlag()
			&& m_indexMapping != component.get() )
		{
			if ( m_indexMapping )
			{
				m_components.erase( m_indexMapping->getId() );
			}

			m_indexMapping = static_cast< IndexMappingRPtr >( component.get() );
		}
		else if ( component->getPlugin().getInstantiationFlag()
			&& m_instantiation != component.get() )
		{
			if ( m_instantiation )
			{
				m_components.erase( m_instantiation->getId() );
			}

			m_instantiation = static_cast< InstantiationComponentRPtr >( component.get() );
		}

		auto id = component->getId();
		m_components.emplace( id, std::move( component ) );
	}

	InterleavedVertex Submesh::getInterleavedPoint( uint32_t index )const
	{
		CU_Require( index < getPointsCount() );
		InterleavedVertex result;

		if ( auto positions = getComponent< PositionsComponent >() )
		{
			result.pos = positions->getData().getData()[index];
		}

		if ( auto normals = getComponent< NormalsComponent >() )
		{
			result.nml = normals->getData().getData()[index];
		}

		if ( auto tangents = getComponent< TangentsComponent >() )
		{
			result.tan = tangents->getData().getData()[index];
		}

		if ( auto texcoords0 = getComponent< Texcoords0Component >() )
		{
			result.tex = texcoords0->getData().getData()[index];
		}

		return result;
	}

	castor::Point3fArray const & Submesh::getPositions()const
	{
		if ( auto component = getComponent< PositionsComponent >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getPositions()
	{
		m_dirty = true;
		auto component = getComponent< PositionsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getNormals()const
	{
		if ( auto component = getComponent< NormalsComponent >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getNormals()
	{
		m_dirty = true;
		auto component = getComponent< NormalsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point4fArray const & Submesh::getTangents()const
	{
		if ( auto component = getComponent< TangentsComponent >() )
		{
			return component->getData().getData();
		}

		static castor::Point4fArray const dummy;
		return dummy;
	}

	castor::Point4fArray & Submesh::getTangents()
	{
		m_dirty = true;
		auto component = getComponent< TangentsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getBitangents()const
	{
		if ( auto component = getComponent< BitangentsComponent >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getBitangents()
	{
		m_dirty = true;
		auto component = getComponent< BitangentsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords0()const
	{
		if ( auto component = getComponent< Texcoords0Component >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords0()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords0Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords1()const
	{
		if ( auto component = getComponent< Texcoords1Component >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords1()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords1Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords2()const
	{
		if ( auto component = getComponent< Texcoords2Component >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords2()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords2Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords3()const
	{
		if ( auto component = getComponent< Texcoords3Component >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords3()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords3Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getColours()const
	{
		if ( auto component = getComponent< ColoursComponent >() )
		{
			return component->getData().getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getColours()
	{
		m_dirty = true;
		auto component = getComponent< ColoursComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	castor::Point3fArray const & Submesh::getBaseData( SubmeshData submeshData )const
	{
		switch ( submeshData )
		{
		case castor3d::SubmeshData::ePositions:
			return getPositions();
		case castor3d::SubmeshData::eNormals:
			return getNormals();
		case castor3d::SubmeshData::eBitangents:
			return getBitangents();
		case castor3d::SubmeshData::eTexcoords0:
			return getTexcoords0();
		case castor3d::SubmeshData::eTexcoords1:
			return getTexcoords1();
		case castor3d::SubmeshData::eTexcoords2:
			return getTexcoords2();
		case castor3d::SubmeshData::eTexcoords3:
			return getTexcoords3();
		case castor3d::SubmeshData::eColours:
			return getColours();
		case castor3d::SubmeshData::eTangents:
			CU_Failure( "getBaseData: Can't retrieve tangent data this way" );
			break;
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
			break;
		case castor3d::SubmeshData::eMeshlets:
			CU_Failure( "getBaseData: Can't retrieve meshlets data this way" );
			break;
		default:
			CU_Failure( "getBaseData: Unsupported SubmeshData type" );
			break;
		}

		static castor::Point3fArray const dummy{};
		return dummy;
	}

	castor::Point3fArray & Submesh::getBaseData( SubmeshData submeshData )
	{
		switch ( submeshData )
		{
		case castor3d::SubmeshData::ePositions:
			return getPositions();
		case castor3d::SubmeshData::eNormals:
			return getNormals();
		case castor3d::SubmeshData::eBitangents:
			return getBitangents();
		case castor3d::SubmeshData::eTexcoords0:
			return getTexcoords0();
		case castor3d::SubmeshData::eTexcoords1:
			return getTexcoords1();
		case castor3d::SubmeshData::eTexcoords2:
			return getTexcoords2();
		case castor3d::SubmeshData::eTexcoords3:
			return getTexcoords3();
		case castor3d::SubmeshData::eColours:
			return getColours();
		case castor3d::SubmeshData::eTangents:
			CU_Failure( "getBaseData: Can't retrieve tangent data this way" );
			break;
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
			break;
		case castor3d::SubmeshData::eMeshlets:
			CU_Failure( "getBaseData: Can't retrieve meshlets data this way" );
			break;
		default:
			CU_Failure( "getBaseData: Unsupported SubmeshData type" );
			break;
		}

		static castor::Point3fArray dummy{};
		return dummy;
	}

	GpuBufferOffsetT< castor::Point4f > const & Submesh::getMorphTargets()const
	{
		if ( auto component = getComponent< MorphComponent >() )
		{
			return component->getData().getMorphTargets();
		}

		static GpuBufferOffsetT< castor::Point4f > const dummy{};
		return dummy;
	}

	uint32_t Submesh::getMorphTargetsCount()const
	{
		if ( auto component = getComponent< MorphComponent >() )
		{
			return component->getData().getMorphTargetsCount();
		}

		return 0u;
	}

	std::vector< Meshlet > const & Submesh::getMeshlets()const
	{
		if ( auto component = getComponent< MeshletComponent >() )
		{
			return component->getData().getMeshletsData();
		}

		static std::vector< Meshlet > const dummy{};
		return dummy;
	}

	std::vector< Meshlet > & Submesh::getMeshlets()
	{
		m_dirty = true;
		auto component = getComponent< MeshletComponent >();

		if ( !component )
		{
			CU_Exception( "Couldn't retrieve Meshlets component." );
		}

		return component->getData().getMeshletsData();
	}

	uint32_t Submesh::getMeshletsCount()const
	{
		if ( auto component = getComponent< MeshletComponent >() )
		{
			return component->getData().getMeshletsCount();
		}

		return 0u;
	}

	bool Submesh::isDynamic()const
	{
		return hasComponent( SkinComponent::TypeName )
			|| hasComponent( MorphComponent::TypeName );
	}

	bool Submesh::isAnimated()const
	{
		return isDynamic()
			&& getOwner()->hasAnimation();
	}

	ObjectBufferOffset const & Submesh::getFinalBufferOffsets( Geometry const & geometry
		, Pass const & pass )const
	{
		if ( !isDynamic() )
		{
			return m_sourceBufferOffset;
		}

		auto it = m_finalBufferOffsets.find( geometry.getHash( pass, *this ) );

		if ( it != m_finalBufferOffsets.end() )
		{
			return it->second;
		}

		log::error << "Couldn't find instance in final buffers" << std::endl;
		CU_Failure( "Couldn't find instance in final buffers" );
		return m_sourceBufferOffset;
	}

	ObjectBufferOffset const & Submesh::getSourceBufferOffsets()const
	{
		return m_sourceBufferOffset;
	}

	GpuBufferOffsetT< MeshletCullData > const & Submesh::getFinalMeshletsBounds( Geometry const & geometry
		, Pass const & pass )const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		CU_Require( meshletComponent );
		return meshletComponent->getData().getFinalCullBuffer( geometry, pass );
	}

	GpuBufferOffsetT< MeshletCullData > const & Submesh::getSourceMeshletsBounds()const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		CU_Require( meshletComponent );
		return meshletComponent->getData().getSourceCullBuffer();
	}

	bool Submesh::hasMorphComponent()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().getMorphFlag() != 0u;
			} );
		return it != m_components.end();
	}

	bool Submesh::hasSkinComponent()const
	{
		auto it = std::find_if( m_components.begin()
			, m_components.end()
			, []( SubmeshComponentIDMap::value_type const & lookup )
			{
				return lookup.second->getPlugin().getSkinFlag() != 0u;
			} );
		return it != m_components.end();
	}

	VkDeviceSize Submesh::getVertexOffset( Geometry const & geometry
		, Pass const & pass )const
	{
		return getFinalBufferOffsets( geometry, pass ).getFirstVertex< castor::Point4f >();
	}

	VkDeviceSize Submesh::getIndexOffset()const
	{
		return m_sourceBufferOffset
			? m_sourceBufferOffset.getFirstIndex< uint32_t >()
			: 0u;
	}

	VkDeviceSize Submesh::getMeshletOffset()const
	{
		return m_sourceBufferOffset
			? m_sourceBufferOffset.getFirst< Meshlet >( SubmeshData::eMeshlets )
			: 0u;
	}

	SubmeshComponentRegister & Submesh::getSubmeshComponentsRegister()const
	{
		return getOwner()->getEngine()->getSubmeshComponentsRegister();
	}

	SubmeshComponentID Submesh::getComponentId( castor::String const & componentType )const
	{
		return getSubmeshComponentsRegister().getNameId( componentType );
	}

	SubmeshComponentPlugin const & Submesh::getComponentPlugin( SubmeshComponentID componentId )const
	{
		return getSubmeshComponentsRegister().getPlugin( componentId );
	}

	SubmeshComponentCombineID Submesh::getComponentCombineID()const noexcept
	{
		CU_Require( m_componentCombine.baseId != 0 );
		return m_componentCombine.baseId;
	}

	bool Submesh::hasRenderComponent()const noexcept
	{
		return m_render != nullptr;
	}

	SubmeshRenderData * Submesh::getRenderData()const
	{
		if ( m_render == nullptr )
		{
			CU_Failure( "Submesh doesn't contain any render shader component" );
			throw std::logic_error{ "Submesh doesn't contain any render shader component" };
		}

		return m_render->getRenderData();
	}

	void Submesh::doInstantiate( Geometry const * geometry
		, MaterialObs oldMaterial
		, MaterialObs newMaterial
		, bool update )
	{
		if ( newMaterial )
		{
			newMaterial->initialise();
		}

		if ( oldMaterial != newMaterial )
		{
			if ( update && m_instantiation )
			{
				auto & data = m_instantiation->getData();

				data.unref( oldMaterial );

				if ( data.ref( newMaterial ) )
				{
					m_geometryBuffers.clear();
				}
			}
		}

		if ( geometry && isDynamic() )
		{
			for ( auto & pass : *newMaterial )
			{
				if ( auto meshletComponent = getComponent< MeshletComponent >() )
				{
					meshletComponent->getData().instantiate( *geometry, *pass );
				}

				auto it = m_finalBufferOffsets.emplace( geometry->getHash( *pass, *this ), ObjectBufferOffset{} ).first;

				if ( m_initialised
					&& !it->second )
				{
					// Initialise only if the submesh itself is already initialised,
					// because if it is not, the buffers will be initialised by the call to initialise().
					ashes::BufferBase const * indexBuffer{};

					if ( m_indexMapping )
					{
						indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshData::eIndex );
					}

					auto combine = getComponentCombine();
					auto & engine = *getOwner()->getEngine();
					auto & components = engine.getSubmeshComponentsRegister();
					remFlags( combine, components.getSkinFlag() );
					RenderDevice & device = engine.getRenderSystem()->getRenderDevice();
					it->second = device.geometryPools->getBuffer( getPointsCount()
						, indexBuffer
						, combine );

					if ( !it->second.hasData( SubmeshData::ePositions )
						|| !it->second.getBufferChunk( SubmeshData::ePositions ).buffer )
					{
						CU_Failure( "No final data available for submesh" );
						CU_Exception( "No final data available for submesh" );
					}
				}
			}
		}
	}

	Engine * getEngine( SubmeshContext const & context )
	{
		return getEngine( *context.mesh );
	}

	//*********************************************************************************************
}
