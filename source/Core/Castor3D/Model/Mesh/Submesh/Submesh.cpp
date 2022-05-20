#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace smsh
	{
		static size_t hash( MaterialRPtr material
			, ShaderFlags const & shaderFlags
			, ProgramFlags const & programFlags
			, SubmeshFlags const & submeshFlags
			, TextureFlagsArray const & mask )
		{
			auto result = std::hash< Material * >{}( material );

			for ( auto const & flagId : mask )
			{
				result = castor::hashCombine( result, flagId.id );
				result = castor::hashCombine( result, flagId.flags.value() );
			}

			result = castor::hashCombine( result, shaderFlags.value() );
			result = castor::hashCombine( result, programFlags.value() );
			result = castor::hashCombine( result, submeshFlags.value() );
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

		if ( checkFlag( flags, SubmeshFlag::eBones ) )
		{
			createComponent< BonesComponent >();
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
			if ( !m_bufferOffset
				|| getPointsCount() != m_bufferOffset.getCount< castor::Point3f >( SubmeshFlag::ePositions ) )
			{
				VkDeviceSize indexCount = 0u;

				if ( m_indexMapping )
				{
					indexCount = VkDeviceSize( m_indexMapping->getCount() ) * m_indexMapping->getComponentsCount();
				}

				for ( auto & component : m_components )
				{
					m_submeshFlags |= component.second->getSubmeshFlags( nullptr );
				}

				m_bufferOffset = device.geometryPools->getBuffer( getPointsCount()
					, indexCount
					, m_submeshFlags );
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

	void Submesh::cleanup( RenderDevice const & device )
	{
		m_initialised = false;

		for ( auto & component : m_components )
		{
			component.second->cleanup( device );
		}

		if ( m_bufferOffset )
		{
			device.geometryPools->putBuffer( m_bufferOffset );
		}
	}

	void Submesh::update()
	{
		if ( m_dirty && m_bufferOffset )
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
		auto positions = getComponent< PositionsComponent >();

		if ( positions && getPointsCount() )
		{
			auto & points = positions->getData();
			castor::Point3f min = points[0];
			castor::Point3f max = points[0];
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				castor::Point3f cur = points[i];
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
				computeNormals( false );
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
			, uint32_t( m_bufferOffset ? m_bufferOffset.getCount< castor::Point3f >( SubmeshFlag::ePositions ) : 0u ) } );
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
				if ( castor::point::distanceSquared( vertex, *it ) < precision )
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

	void Submesh::sortByDistance( castor::Point3f const & cameraPosition )
	{
		if ( m_indexMapping )
		{
			m_indexMapping->sortByDistance( cameraPosition );
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

	void Submesh::setMaterial( MaterialRPtr oldMaterial
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
	}

	GeometryBuffers const & Submesh::getGeometryBuffers( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, SubmeshFlags const & submeshFlags
		, MaterialRPtr material
		, TextureFlagsArray const & mask )const
	{
		auto key = smsh::hash( material
			, shaderFlags
			, programFlags
			, submeshFlags
			, mask );
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
				component.second->gather( shaderFlags
					, programFlags
					, submeshFlags
					, material
					, mask
					, buffers
					, offsets
					, layouts
					, currentBinding
					, currentLocation );
			}

			GeometryBuffers result;
			result.bufferOffset = m_bufferOffset;
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

	InterleavedVertex Submesh::getInterleavedPoint( uint32_t index )const
	{
		CU_Require( index < getPointsCount() );
		InterleavedVertex result;

		if ( auto positions = getComponent< PositionsComponent >() )
		{
			result.pos = positions->getData()[index];
		}

		if ( auto normals = getComponent< NormalsComponent >() )
		{
			result.nml = normals->getData()[index];
		}

		if ( auto tangents = getComponent< TangentsComponent >() )
		{
			result.tan = tangents->getData()[index];
		}

		if ( auto texcoords0 = getComponent< Texcoords0Component >() )
		{
			result.tex = texcoords0->getData()[index];
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
		auto component = getComponent< ColoursComponent >();
		CU_Require( component );
		return component->getData();
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

	//*********************************************************************************************
}
