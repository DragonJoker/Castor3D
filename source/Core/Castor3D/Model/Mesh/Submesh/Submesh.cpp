#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace smsh
	{
		static size_t hash( SubmeshRenderNode const & node
			, PipelineFlags const & flags )
		{
			size_t result = node.data.isDynamic()
				? std::hash< Geometry const * >{}( &node.instance )
				: std::hash< MaterialRPtr >{}( node.pass->getOwner() );

			//for ( auto const & flagId : flags.m_texturesFlags )
			//{
			//	result = castor::hashCombine( result, flagId.id );
			//	result = castor::hashCombine( result, flagId.flags.value() );
			//}

			result = castor::hashCombine( result, flags.m_shaderFlags.value() );
			result = castor::hashCombine( result, flags.m_programFlags.value() );
			result = castor::hashCombine( result, flags.m_submeshFlags.value() );
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
			auto comp = submesh.getComponent< ComponentT >();

			if ( comp )
			{
				result = uint32_t( comp->getData().size() );
			}

			return result;
		}

		template< typename ComponentT, typename DataT >
		void addComponentData( Submesh & submesh
			, DataT const & data )
		{
			auto comp = submesh.getComponent< ComponentT >();

			if ( comp )
			{
				comp->getData().push_back( data );
			}
		}

		template< typename ComponentT >
		void reserveComponentData( Submesh & submesh
			, uint32_t size )
		{
			auto comp = submesh.getComponent< ComponentT >();

			if ( comp )
			{
				comp->getData().reserve( size );
			}
		}
	}

	//*********************************************************************************************

	Submesh::Submesh( Mesh & mesh
		, uint32_t id
		, SubmeshFlags const & flags )
		: OwnedBy< Mesh >{ mesh }
		, m_id{ id }
		, m_defaultMaterial{ mesh.getScene()->getEngine()->getMaterialCache().getDefaultMaterial() }
		, m_submeshFlags{ SubmeshFlag::eIndex | flags }
	{
		addComponent( std::make_shared< InstantiationComponent >( *this, 2u ) );

		if ( checkFlag( flags, SubmeshFlag::ePositions ) )
		{
			createComponent< PositionsComponent >();
		}

		if ( checkFlag( flags, SubmeshFlag::eNormals ) )
		{
			createComponent< NormalsComponent >();
		}

		if ( checkFlag( flags, SubmeshFlag::eTangents ) )
		{
			createComponent< TangentsComponent >();
		}

		if ( checkFlag( flags, SubmeshFlag::eTexcoords0 ) )
		{
			createComponent< Texcoords0Component >();
		}

		if ( checkFlag( flags, SubmeshFlag::eTexcoords1 ) )
		{
			createComponent< Texcoords1Component >();
		}

		if ( checkFlag( flags, SubmeshFlag::eTexcoords2 ) )
		{
			createComponent< Texcoords2Component >();
		}

		if ( checkFlag( flags, SubmeshFlag::eTexcoords3 ) )
		{
			createComponent< Texcoords3Component >();
		}

		if ( checkFlag( flags, SubmeshFlag::eColours ) )
		{
			createComponent< ColoursComponent >();
		}

		if ( checkFlag( flags, SubmeshFlag::eSkin ) )
		{
			createComponent< SkinComponent >();
		}

		if ( checkFlag( flags, SubmeshFlag::ePassMasks ) )
		{
			createComponent< PassMasksComponent >();
		}
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
				|| getPointsCount() != m_sourceBufferOffset.getCount< castor::Point4f >( SubmeshFlag::ePositions ) )
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
					&& !hasComponent( BaseDataComponentT< SubmeshFlag::eVelocity >::Name ) )
				{
					createComponent< BaseDataComponentT< SubmeshFlag::eVelocity > >();
				}

				for ( auto & component : m_components )
				{
					m_submeshFlags |= component.second->getSubmeshFlags( nullptr );
				}

				auto flags = m_submeshFlags;
				remFlag( flags, SubmeshFlag::eVelocity );
				m_sourceBufferOffset = device.geometryPools->getBuffer( getPointsCount()
					, indexCount
					, flags );

				if ( isDynamic() )
				{
					ashes::BufferBase const * indexBuffer{};

					if ( m_indexMapping )
					{
						indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshFlag::eIndex );
					}

					flags = m_submeshFlags;
					remFlag( flags, SubmeshFlag::eSkin );

					for ( auto & finalBufferOffset : m_finalBufferOffsets )
					{
						finalBufferOffset.second = device.geometryPools->getBuffer( getPointsCount()
							, indexBuffer
							, flags );
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
					m_initialised = component.second->initialise( device );
				}

				if ( m_initialised )
				{
					component.second->upload();
				}
			}

			m_dirty = !m_initialised;
		}
	}

	VkDeviceSize Submesh::getVertexOffset( Geometry const & geometry )const
	{
		CU_Require( m_sourceBufferOffset );
		return m_sourceBufferOffset
			? getFinalBufferOffsets( geometry ).getFirstVertex< castor::Point4f >()
			: 0u;
	}

	VkDeviceSize Submesh::getIndexOffset()const
	{
		CU_Require( m_sourceBufferOffset );
		return m_sourceBufferOffset
			? m_sourceBufferOffset.getFirstIndex< uint32_t >()
			: 0u;
	}

	VkDeviceSize Submesh::getMeshletOffset()const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		VkDeviceSize result{};

		if ( meshletComponent )
		{
			result = meshletComponent->getMeshletsBuffer().getOffset();
		}

		return result;
	}

	void Submesh::cleanup( RenderDevice const & device )
	{
		m_initialised = false;

		for ( auto & component : m_components )
		{
			component.second->cleanup( device );
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

	void Submesh::update()
	{
		if ( m_dirty && m_sourceBufferOffset )
		{
			for ( auto & component : m_components )
			{
				component.second->upload();
			}

			m_dirty = false;
		}

		for ( auto & component : m_components )
		{
			component.second->upload();
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
			auto & points = positions->getData();
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
			, smsh::getComponentCount< Texcoords0Component >( *this )
			, smsh::getComponentCount< Texcoords1Component >( *this )
			, smsh::getComponentCount< Texcoords2Component >( *this )
			, smsh::getComponentCount< Texcoords3Component >( *this )
			, smsh::getComponentCount< ColoursComponent >( *this )
			, uint32_t( m_sourceBufferOffset ? m_sourceBufferOffset.getCount< castor::Point4f >( SubmeshFlag::ePositions ) : 0u ) } );
	}

	int Submesh::isInMyPoints( castor::Point3f const & vertex
		, double precision )
	{
		int result = -1;
		auto positions = getComponent< PositionsComponent >();

		if ( positions )
		{
			int index = 0;
			auto & points = positions->getData();

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
			m_needsNormalsCompute = false;
		}
	}

	ProgramFlags Submesh::getProgramFlags( Material const & material )const
	{
		ProgramFlags result{};

		for ( auto & component : m_components )
		{
			result |= component.second->getProgramFlags( material );
		}

		return result;
	}

	SubmeshFlags Submesh::getSubmeshFlags( Pass const * pass )const
	{
		SubmeshFlags result{};

		for ( auto & component : m_components )
		{
			result |= component.second->getSubmeshFlags( pass );
		}

		return result;
	}

	MorphFlags Submesh::getMorphFlags()const
	{
		MorphFlags result{};

		if ( auto morph = getComponent< MorphComponent >() )
		{
			result = morph->getMorphFlags();
		}

		return result;
	}

	void Submesh::instantiate( Geometry const * geometry
		, MaterialRPtr oldMaterial
		, MaterialRPtr newMaterial
		, bool update )
	{
		if ( oldMaterial != newMaterial )
		{
			if ( update && m_instantiation )
			{
				m_instantiation->unref( oldMaterial );

				if ( m_instantiation->ref( newMaterial ) )
				{
					m_geometryBuffers.clear();
				}
			}
		}

		if ( geometry && isDynamic() )
		{
			if ( auto meshletComponent = getComponent< MeshletComponent >() )
			{
				meshletComponent->instantiate( *geometry );
			}

			auto it = m_finalBufferOffsets.emplace( geometry, ObjectBufferOffset{} ).first;

			if ( m_initialised
				&& !it->second )
			{
				// Initialise only if the submesh itself is already initialised,
				// because if it is not, the buffers will be initialised by the call to initialise().
				ashes::BufferBase const * indexBuffer{};

				if ( m_indexMapping )
				{
					indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshFlag::eIndex );
				}

				auto flags = m_submeshFlags;
				remFlag( flags, SubmeshFlag::eSkin );
				RenderDevice & device = getOwner()->getOwner()->getRenderSystem()->getRenderDevice();
				it->second = device.geometryPools->getBuffer( getPointsCount()
					, indexBuffer
					, flags );
			}
		}
	}

	GeometryBuffers const & Submesh::getGeometryBuffers( SubmeshRenderNode const & node
		, PipelineFlags const & flags )const
	{
		auto key = smsh::hash( node, flags );
		auto it = m_geometryBuffers.find( key );

		if ( it == m_geometryBuffers.end() )
		{
			ashes::BufferCRefArray buffers;
			ashes::UInt64Array offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			uint32_t currentBinding = 0u;
			uint32_t currentLocation = 0u;

			for ( auto & component : m_components )
			{
				component.second->gather( flags
					, node.pass->getOwner()
					, buffers
					, offsets
					, layouts
					, currentBinding
					, currentLocation );
			}

			GeometryBuffers result;
			result.indexOffset = getSourceBufferOffsets().getBufferChunk( SubmeshFlag::eIndex );
			result.bufferOffset = getFinalBufferOffsets( node.instance );
			result.layouts = layouts;
			result.other = buffers;
			result.otherOffsets = offsets;

			it = m_geometryBuffers.emplace( key, std::move( result ) ).first;
		}

		return it->second;
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
		case castor3d::SubmeshData::eTangents:
			getTangents() = std::move( data );
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
		default:
			CU_Failure( "setBaseData: Unsupported SubmeshData type" );
			break;
		}
	}

	InterleavedVertex Submesh::getInterleavedPoint( uint32_t index )const
	{
		CU_Require( index < getPointsCount() );
		InterleavedVertex result;

		if ( auto positions = getComponent< PositionsComponent >() )
		{
			result.pos = castor::Point3f{ positions->getData()[index] };
		}

		if ( auto normals = getComponent< NormalsComponent >() )
		{
			result.nml = castor::Point3f{ normals->getData()[index] };
		}

		if ( auto tangents = getComponent< TangentsComponent >() )
		{
			result.tan = castor::Point3f{ tangents->getData()[index] };
		}

		if ( auto texcoords0 = getComponent< Texcoords0Component >() )
		{
			result.tex = castor::Point3f{ texcoords0->getData()[index] };
		}

		return result;
	}

	castor::Point3fArray const & Submesh::getPositions()const
	{
		if ( auto component = getComponent< PositionsComponent >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getPositions()
	{
		m_dirty = true;
		auto component = getComponent< PositionsComponent >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getNormals()const
	{
		if ( auto component = getComponent< NormalsComponent >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getNormals()
	{
		m_dirty = true;
		auto component = getComponent< NormalsComponent >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getTangents()const
	{
		if ( auto component = getComponent< TangentsComponent >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTangents()
	{
		m_dirty = true;
		auto component = getComponent< TangentsComponent >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords0()const
	{
		if ( auto component = getComponent< Texcoords0Component >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords0()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords0Component >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords1()const
	{
		if ( auto component = getComponent< Texcoords1Component >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords1()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords1Component >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords2()const
	{
		if ( auto component = getComponent< Texcoords2Component >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords2()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords2Component >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getTexcoords3()const
	{
		if ( auto component = getComponent< Texcoords3Component >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getTexcoords3()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords3Component >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getColours()const
	{
		if ( auto component = getComponent< ColoursComponent >() )
		{
			return component->getData();
		}

		static castor::Point3fArray const dummy;
		return dummy;
	}

	castor::Point3fArray & Submesh::getColours()
	{
		m_dirty = true;
		auto component = getComponent< ColoursComponent >();
		CU_Require( component );
		return component->getData();
	}

	castor::Point3fArray const & Submesh::getBaseData( SubmeshData submeshData )const
	{
		switch ( submeshData )
		{
		case castor3d::SubmeshData::ePositions:
			return getPositions();
		case castor3d::SubmeshData::eNormals:
			return getNormals();
		case castor3d::SubmeshData::eTangents:
			return getTangents();
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
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
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
		case castor3d::SubmeshData::eTangents:
			return getTangents();
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
		case castor3d::SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case castor3d::SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case castor3d::SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
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
			return component->getMorphTargets();
		}

		static GpuBufferOffsetT< castor::Point4f > const dummy{};
		return dummy;
	}

	uint32_t Submesh::getMorphTargetsCount()const
	{
		if ( auto component = getComponent< MorphComponent >() )
		{
			return component->getMorphTargetsCount();
		}

		return 0u;
	}

	std::vector< Meshlet > const & Submesh::getMeshlets()const
	{
		if ( auto component = getComponent< MeshletComponent >() )
		{
			return component->getMeshletsData();
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

		return component->getMeshletsData();
	}

	uint32_t Submesh::getMeshletsCount()const
	{
		if ( auto component = getComponent< MeshletComponent >() )
		{
			return component->getMeshletsCount();
		}

		return 0u;
	}

	bool Submesh::isDynamic()const
	{
		return hasComponent( SkinComponent::Name )
			|| hasComponent( MorphComponent::Name );
	}

	ObjectBufferOffset const & Submesh::getFinalBufferOffsets( Geometry const & instance )const
	{
		if ( !isDynamic() )
		{
			CU_Require( bool( m_sourceBufferOffset ) );
			return m_sourceBufferOffset;
		}

		auto it = m_finalBufferOffsets.find( &instance );

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
		CU_Require( bool( m_sourceBufferOffset ) );
		return m_sourceBufferOffset;
	}

	GpuBufferOffsetT< Meshlet > const & Submesh::getMeshletsBuffer()const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		CU_Require( meshletComponent );
		return meshletComponent->getMeshletsBuffer();
	}

	GpuBufferOffsetT< MeshletCullData > const & Submesh::getFinalMeshletsBounds( Geometry const & instance )const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		CU_Require( meshletComponent );
		return meshletComponent->getFinalCullBuffer( instance );
	}

	GpuBufferOffsetT< MeshletCullData > const & Submesh::getSourceMeshletsBounds()const
	{
		auto meshletComponent = getComponent< MeshletComponent >();
		CU_Require( meshletComponent );
		return meshletComponent->getSourceCullBuffer();
	}

	bool Submesh::hasMorphComponent()const
	{
		return hasComponent( MorphComponent::Name );
	}

	bool Submesh::hasSkinComponent()const
	{
		return hasComponent( SkinComponent::Name );
	}

	//*********************************************************************************************
}
