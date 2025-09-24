#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MeshletComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( c3d, Submesh )

namespace c3d
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
			result = hashCombine( result, flags.m_shaderFlags.value() );
			result = hashCombine( result, flags.m_programFlags.value() );
			result = hashCombine( result, flags.submesh.baseId );
			return result;
		}

		static bool fix( Point3f & value
			, Point3f const & defaultValue )
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

		static bool fixNml( Point3f & value )
		{
			static Point3f const defaultValue{ 0.0f, 1.0f, 0.0f };
			auto result = fix( value, defaultValue );

			if ( point::length( value ) < std::numeric_limits< float >::epsilon() )
			{
				value = defaultValue;
				result = true;
			}

			return result;
		}

		static bool fixPos( Point3f & value )
		{
			static Point3f const defaultValue{ 0.0f, 0.0f, 0.0f };
			return fix( value, defaultValue );
		}

		static bool fixTex( Point3f & value )
		{
			static Point3f const defaultValue{ 0.0f, 0.0f, 0.0f };
			return fix( value, defaultValue );
		}

		template< typename ComponentT >
		static uint32_t getComponentCount( Submesh const & submesh )
		{
			uint32_t result{};

			if ( ComponentT const * comp = submesh.getComponent< ComponentT >() )
			{
				if ( SubmeshComponentData const * compData = comp->getBaseData() )
				{
					result = uint32_t( static_cast< const typename ComponentT::ComponentData * >( compData )->getData().size() );
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

		static MbString getDescriptorName( Submesh const & submesh )
		{
			return toUtf8( submesh.getOwner()->getName() )
				+ string::toMbString( submesh.getId() )
				+ "Mesh";
		}

		static CU_ImplementAttributeParserNewBlock( parserMeshSubmesh, MeshContext, SubmeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				newBlockContext->mesh = blockContext;
				newBlockContext->submesh = blockContext->mesh->createSubmesh();
				newBlockContext->submesh->createComponent< DefaultRenderComponent >();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSubmesh )

		static CU_ImplementAttributeParserBlock( parserSubmeshEnd, SubmeshContext )
		{
			if ( blockContext->submesh->getPointsCount() > 0
				&& blockContext->submesh->getFaceCount() > 0 )
			{
				MeshPreparer::prepare( *blockContext->submesh
					, Parameters{} );
				blockContext->submesh->computeContainers();
				blockContext->submesh->getParent().getScene()->getListener().postEvent( makeGpuInitialiseEvent( *blockContext->submesh ) );
			}
			else
			{
				blockContext->mesh->mesh->removeSubmesh( *blockContext->submesh );
			}

			blockContext->submesh = {};
		}
		CU_EndAttributePop()
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

	Submesh::~Submesh()noexcept
	{
		CU_Assert( !m_initialised, "Did you forget to call Submesh::cleanup ?" );
	}

	void Submesh::initialise( RenderDevice const & device )
	{
		if ( !m_generated )
		{
			if ( !m_sourceBufferOffset
				|| getPointsCount() != m_sourceBufferOffset.getCount< Point4f >( SubmeshData::ePositions ) )
			{
				for ( auto & [_, finalBufferOffset] : m_finalBufferOffsets )
				{
					if ( finalBufferOffset )
					{
						device.geometryPools->putBuffer( finalBufferOffset );
						finalBufferOffset = {};
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
					BufferBase const * indexBuffer{};
					if ( m_indexMapping )
						indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshData::eIndex );

					combine = m_componentCombine;
					remFlags( combine, components.getSkinFlag() );
					combine.hasSkinFlag = false;
					components.registerSubmeshComponentCombine( combine );

					for ( auto & [_, finalBufferOffset] : m_finalBufferOffsets )
					{
						finalBufferOffset = device.geometryPools->getBuffer( getPointsCount()
							, indexBuffer
							, combine );

						if ( !finalBufferOffset.hasData( SubmeshData::ePositions )
							|| !finalBufferOffset.getBufferChunk( SubmeshData::ePositions ).buffer )
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

			for ( auto const & [_, component] : m_components )
			{
				if ( m_initialised )
				{
					if ( auto data = component->getBaseData() )
					{
						m_initialised = data->initialise( device );
					}
				}

				if ( m_initialised )
				{
					if ( auto data = component->getRenderData() )
					{
						m_initialised = data->initialise( device );
					}
				}
			}

			doCreateDescriptorLayout( device );
			m_dirty = !m_initialised;
		}
	}

	void Submesh::cleanup( RenderDevice const & device )
	{
		m_initialised = false;

		m_descriptorSets.clear();
		m_descriptorPool.reset();
		m_descriptorLayout.reset();

		for ( auto const & [_, component] : m_components )
		{
			if ( auto data = component->getRenderData() )
			{
				data->cleanup( device );
			}

			if ( auto data = component->getBaseData() )
			{
				data->cleanup( device );
			}
		}

		for ( auto & [_, finalBufferOffset] : m_finalBufferOffsets )
		{
			device.geometryPools->putBuffer( finalBufferOffset );
			finalBufferOffset = {};
		}

		if ( m_sourceBufferOffset )
		{
			device.geometryPools->putBuffer( m_sourceBufferOffset );
		}
	}

	void Submesh::upload( UploadData & uploader )
	{
		for ( auto const & [_, component] : m_components )
		{
			if ( auto data = component->getBaseData() )
			{
				m_dirty = m_dirty || data->isDirty();
				data->upload( uploader );
			}
		}
	}

	bool Submesh::update( CpuUpdater & updater )
	{
		for ( auto const & [_, component] : m_components )
		{
			if ( auto data = component->getRenderData() )
			{
				data->update( updater );
			}
		}

		bool result = m_dirty;
		m_dirty = false;
		return result;
	}

	void Submesh::record( crg::ResourcesCache & resources
		, crg::FramePassGroup & graph )const
	{
		auto const & device = *getParent().getOwner()->getRenderDevice();
		for ( auto const & [_, component] : m_components )
			if( auto data = component->getRenderData() )
				data->record( device, resources, graph );
	}

	void Submesh::registerDependencies( crg::FramePass & pass )const
	{
		for ( auto const & [_, component] : m_components )
		{
			if ( auto data = component->getRenderData() )
			{
				data->registerDependencies( pass );
			}
		}
	}

	void Submesh::accept( ConfigurationVisitorBase & vis )
	{
		StringArray topologies;
		topologies.emplace_back( cuT( "Point List" ) );
		topologies.emplace_back( cuT( "Line List" ) );
		topologies.emplace_back( cuT( "Line Strip" ) );
		topologies.emplace_back( cuT( "Triangle List" ) );
		topologies.emplace_back( cuT( "Triangle Strip" ) );
		topologies.emplace_back( cuT( "Triangle Fan" ) );
		topologies.emplace_back( cuT( "Line List With Adjacency" ) );
		topologies.emplace_back( cuT( "Line Strip With Adjacency" ) );
		topologies.emplace_back( cuT( "Triangle List With Adjacency" ) );
		topologies.emplace_back( cuT( "Triangle Strip With Adjacency" ) );
		topologies.emplace_back( cuT( "Patch List" ) );
		vis.visit< VkPrimitiveTopology >( cuT( "Topology" ), m_topology, topologies
			, [this]( int, int newV )
			{
				m_topology = VkPrimitiveTopology( newV );
			} );

		for ( auto const & [_, component] : m_components )
		{
			component->accept( vis );
		}
	}

	void Submesh::computeContainers()
	{
		if ( !m_dirty )
		{
			return;
		}

		PositionsComponent const * positions = getComponent< PositionsComponent >();

		if ( positions && getPointsCount() )
		{
			auto const & data = positions->getData();
			auto & points = data.getData();
			Point3f min{ points[0] };
			Point3f max{ points[0] };
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				Point3f cur{ points[i] };
				max->x = std::max( cur->x, max->x );
				max->y = std::max( cur->y, max->y );
				max->z = std::max( cur->z, max->z );
				min->x = std::min( cur->x, min->x );
				min->y = std::min( cur->y, min->y );
				min->z = std::min( cur->z, min->z );
			}

			m_box.load( min, max );
			m_sphere.load( m_box );

			if ( m_needsNormalsCompute )
			{
				computeNormals();
			}
		}
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
			, uint32_t( m_sourceBufferOffset ? m_sourceBufferOffset.getCount< Point4f >( SubmeshData::ePositions ) : 0u ) } );
	}

	int Submesh::isInMyPoints( Point3f const & vertex
		, double precision )const
	{
		int result = -1;

		if ( auto positions = getComponent< PositionsComponent >() )
		{
			int index = 0;
			auto & points = positions->getData().getData();
			auto it = points.begin();
			while( it != points.end() && result == -1 )
			{
				if ( point::distanceSquared( vertex, Point3f{ *it } ) < precision )
					result = index;
				++index;
				++it;
			}
		}

		return result;
	}

	InterleavedVertex Submesh::addPoint( float x, float y, float z )
	{
		InterleavedVertex result;
		result.pos = Point3f{ x, y, z };
		addPoint( result );
		return result;
	}

	InterleavedVertex Submesh::addPoint( Point3f const & value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	InterleavedVertex Submesh::addPoint( float const * value )
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

		for ( auto & point : makeArrayView( begin, end ) )
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

		for ( auto const & [_, component] : m_components )
		{
			result |= component->getProgramFlags( pass );
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
		auto [it, res] = m_geometryBuffers.try_emplace( key );

		if ( res )
		{
			ashes::BufferCRefArray buffers;
			ashes::UInt64Array offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			uint32_t currentBinding = 0u;
			uint32_t currentLocation = 0u;
			auto & bufferOffsets = getFinalBufferOffsets( geometry, pass );

			for ( auto const & [_, component] : m_components )
			{
				if ( auto data = component->getBaseData() )
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

			auto & result = it->second;
			result.indexOffset = getSourceBufferOffsets().getBufferChunk( SubmeshData::eIndex );
			result.layouts = layouts;
			result.buffers = buffers;
			result.offsets = offsets;
		}

		return it->second;
	}

	void Submesh::createDescriptorSet( Geometry const & geometry
		, Pass const & pass )
	{
		auto descSetIt = m_descriptorSets.emplace( geometry.getHash( pass, *this ), nullptr ).first;

		if ( !descSetIt->second )
		{
			auto & baseBuffers = getFinalBufferOffsets( geometry, pass );
			descSetIt->second = m_descriptorPool->createDescriptorSet( smsh::getDescriptorName( *this )
				, RenderPipeline::eMeshBuffers );
			ashes::WriteDescriptorSetArray writes;
			auto combine = getComponentCombine();

			auto getStorageBinding = [&baseBuffers]( SubmeshData data, MeshBuffersIdx index )
				{
					auto chunk = baseBuffers.getBufferChunk( data );
					auto binding = ashes::WriteDescriptorSet{ uint32_t( index )
						, 0u
						, 1u
						, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
					binding.bufferInfo.push_back( VkDescriptorBufferInfo{ chunk.buffer->getBuffer().getBuffer()
						, 0u
						, chunk.buffer->getBuffer().getSize() } );
					return binding;
				};

			if ( combine.hasPositionFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::ePositions
					, MeshBuffersIdx::ePosition ) );
			}

			if ( combine.hasNormalFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eNormals
					, MeshBuffersIdx::eNormal ) );
			}

			if ( combine.hasTangentFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eTangents
					, MeshBuffersIdx::eTangent ) );
			}

			if ( combine.hasBitangentFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eBitangents
					, MeshBuffersIdx::eBitangent ) );
			}

			if ( combine.hasTexcoord0Flag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eTexcoords0
					, MeshBuffersIdx::eTexcoord0 ) );
			}

			if ( combine.hasTexcoord1Flag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eTexcoords1
					, MeshBuffersIdx::eTexcoord1 ) );
			}

			if ( combine.hasTexcoord2Flag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eTexcoords2
					, MeshBuffersIdx::eTexcoord2 ) );
			}

			if ( combine.hasTexcoord3Flag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eTexcoords3
					, MeshBuffersIdx::eTexcoord3 ) );
			}

			if ( combine.hasColourFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eColours
					, MeshBuffersIdx::eColour ) );
			}

			if ( combine.hasPassMaskFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::ePassMasks
					, MeshBuffersIdx::ePassMasks ) );
			}

			if ( combine.hasVelocityFlag )
			{
				writes.push_back( getStorageBinding( SubmeshData::eVelocity
					, MeshBuffersIdx::eVelocity ) );
			}

			auto & data = getInstantiation().getData();
			auto bufferIt = data.find( pass );
			CU_Require( bufferIt != data.end() );

			if ( bufferIt->second.buffer )
			{
				writes.push_back( bufferIt->second.buffer.getStorageBinding( uint32_t( MeshBuffersIdx::eInstances ) ) );
			}

			descSetIt->second->setBindings( c3d::move( writes ) );
			descSetIt->second->update();
		}
	}

	ashes::DescriptorSet const & Submesh::getDescriptorSet( Geometry const & geometry
		, Pass const & pass )const
	{
		auto it = m_descriptorSets.find( geometry.getHash( pass, *this ) );
		CU_Require( it != m_descriptorSets.end() );
		return *it->second;
	}

	void Submesh::cloneInto( Submesh & output )const
	{
		output.m_defaultMaterial = m_defaultMaterial;
		output.m_box = m_box;
		output.m_sphere = m_sphere;
		output.m_topology = m_topology;
		output.m_needsNormalsCompute = m_needsNormalsCompute;

		for ( auto & [_, component] : m_components )
		{
			output.addComponent( component->clone( output ) );
		}
	}

	void Submesh::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SubmeshContext > meshContext{ result, CSCNSection::eMesh };
		BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };

		meshContext.addPushParser( cuT( "submesh" ), CSCNSection::eSubmesh, smsh::parserMeshSubmesh );

		submeshContext.addPopParser( cuT( "}" ), smsh::parserSubmeshEnd );
	}

	void Submesh::enableSceneUpdate( bool )
	{
		m_disableSceneUpdate = false;
	}

	void Submesh::setBaseData( SubmeshData submeshData, Point3fArray data )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			getPositions() = c3d::move( data );
			break;
		case SubmeshData::eNormals:
			getNormals() = c3d::move( data );
			break;
		case SubmeshData::eTexcoords0:
			getTexcoords0() = c3d::move( data );
			break;
		case SubmeshData::eTexcoords1:
			getTexcoords1() = c3d::move( data );
			break;
		case SubmeshData::eTexcoords2:
			getTexcoords2() = c3d::move( data );
			break;
		case SubmeshData::eTexcoords3:
			getTexcoords3() = c3d::move( data );
			break;
		case SubmeshData::eColours:
			getColours() = c3d::move( data );
			break;
		case SubmeshData::eBitangents:
			getBitangents() = c3d::move( data );
			break;
		case SubmeshData::eTangents:
			CU_Failure( "setBaseData: Can't set tangent data this way, use the Point4f variant" );
			break;
		case SubmeshData::eIndex:
			CU_Failure( "setBaseData: Can't set index data this way" );
			break;
		case SubmeshData::eSkin:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case SubmeshData::ePassMasks:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case SubmeshData::eVelocity:
			CU_Failure( "setBaseData: Can't set velocity data this way" );
			break;
		case SubmeshData::eMeshlets:
			CU_Failure( "setBaseData: Can't set meshlets data this way" );
			break;
		default:
			CU_Failure( "setBaseData: Unsupported SubmeshData type" );
			break;
		}
	}

	void Submesh::setBaseData( SubmeshData submeshData, Point4fArray data )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			CU_Failure( "setBaseData: Can't set positions data this way, use the Point3f variant" );
			break;
		case SubmeshData::eNormals:
			CU_Failure( "setBaseData: Can't set normals data this way, use the Point3f variant" );
			break;
		case SubmeshData::eTangents:
			getTangents() = c3d::move( data );
			break;
		case SubmeshData::eBitangents:
			CU_Failure( "setBaseData: Can't set bitangents data this way, use the Point3f variant" );
			break;
		case SubmeshData::eTexcoords0:
			CU_Failure( "setBaseData: Can't set texcoords0 data this way, use the Point3f variant" );
			break;
		case SubmeshData::eTexcoords1:
			CU_Failure( "setBaseData: Can't set texcoords1 data this way, use the Point3f variant" );
			break;
		case SubmeshData::eTexcoords2:
			CU_Failure( "setBaseData: Can't set texcoords2 data this way, use the Point3f variant" );
			break;
		case SubmeshData::eTexcoords3:
			CU_Failure( "setBaseData: Can't set texcoords3 data this way, use the Point3f variant" );
			break;
		case SubmeshData::eColours:
			CU_Failure( "setBaseData: Can't set colours data this way, use the Point3f variant" );
			break;
		case SubmeshData::eIndex:
			CU_Failure( "setBaseData: Can't set index data this way" );
			break;
		case SubmeshData::eSkin:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case SubmeshData::ePassMasks:
			CU_Failure( "setBaseData: Can't set skin data this way" );
			break;
		case SubmeshData::eVelocity:
			CU_Failure( "setBaseData: Can't set velocity data this way" );
			break;
		case SubmeshData::eMeshlets:
			CU_Failure( "setBaseData: Can't set meshlets data this way" );
			break;
		default:
			CU_Failure( "setBaseData: Unsupported SubmeshData type" );
			break;
		}
	}

	SubmeshComponentRPtr Submesh::addComponent( SubmeshComponentUPtr component )
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
		return m_components.emplace( id, c3d::move( component ) ).first->second.get();
	}

	void Submesh::setIndexCount( uint32_t value )const
	{
		if ( auto indexMapping = getIndexMapping() )
		{
			indexMapping->setCount( value / indexMapping->getComponentsCount() );
		}
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

	Point3fArray const & Submesh::getPositions()const
	{
		if ( auto component = getComponent< PositionsComponent >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getPositions()
	{
		m_dirty = true;
		auto component = getComponent< PositionsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getNormals()const
	{
		if ( auto component = getComponent< NormalsComponent >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getNormals()
	{
		m_dirty = true;
		auto component = getComponent< NormalsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point4fArray const & Submesh::getTangents()const
	{
		if ( auto component = getComponent< TangentsComponent >() )
		{
			return component->getData().getData();
		}

		static Point4fArray const dummy;
		return dummy;
	}

	Point4fArray & Submesh::getTangents()
	{
		m_dirty = true;
		auto component = getComponent< TangentsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getBitangents()const
	{
		if ( auto component = getComponent< BitangentsComponent >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getBitangents()
	{
		m_dirty = true;
		auto component = getComponent< BitangentsComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getTexcoords0()const
	{
		if ( auto component = getComponent< Texcoords0Component >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getTexcoords0()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords0Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getTexcoords1()const
	{
		if ( auto component = getComponent< Texcoords1Component >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getTexcoords1()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords1Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getTexcoords2()const
	{
		if ( auto component = getComponent< Texcoords2Component >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getTexcoords2()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords2Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getTexcoords3()const
	{
		if ( auto component = getComponent< Texcoords3Component >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getTexcoords3()
	{
		m_dirty = true;
		auto component = getComponent< Texcoords3Component >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getColours()const
	{
		if ( auto component = getComponent< ColoursComponent >() )
		{
			return component->getData().getData();
		}

		static Point3fArray const dummy;
		return dummy;
	}

	Point3fArray & Submesh::getColours()
	{
		m_dirty = true;
		auto component = getComponent< ColoursComponent >();
		CU_Require( component );
		return component->getData().getData();
	}

	Point3fArray const & Submesh::getBaseData( SubmeshData submeshData )const
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			return getPositions();
		case SubmeshData::eNormals:
			return getNormals();
		case SubmeshData::eBitangents:
			return getBitangents();
		case SubmeshData::eTexcoords0:
			return getTexcoords0();
		case SubmeshData::eTexcoords1:
			return getTexcoords1();
		case SubmeshData::eTexcoords2:
			return getTexcoords2();
		case SubmeshData::eTexcoords3:
			return getTexcoords3();
		case SubmeshData::eColours:
			return getColours();
		case SubmeshData::eTangents:
			CU_Failure( "getBaseData: Can't retrieve tangent data this way" );
			break;
		case SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
			break;
		case SubmeshData::eMeshlets:
			CU_Failure( "getBaseData: Can't retrieve meshlets data this way" );
			break;
		default:
			CU_Failure( "getBaseData: Unsupported SubmeshData type" );
			break;
		}

		static Point3fArray const dummy{};
		return dummy;
	}

	Point3fArray & Submesh::getBaseData( SubmeshData submeshData )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			return getPositions();
		case SubmeshData::eNormals:
			return getNormals();
		case SubmeshData::eBitangents:
			return getBitangents();
		case SubmeshData::eTexcoords0:
			return getTexcoords0();
		case SubmeshData::eTexcoords1:
			return getTexcoords1();
		case SubmeshData::eTexcoords2:
			return getTexcoords2();
		case SubmeshData::eTexcoords3:
			return getTexcoords3();
		case SubmeshData::eColours:
			return getColours();
		case SubmeshData::eTangents:
			CU_Failure( "getBaseData: Can't retrieve tangent data this way" );
			break;
		case SubmeshData::eIndex:
			CU_Failure( "getBaseData: Can't retrieve index data this way" );
			break;
		case SubmeshData::eSkin:
			CU_Failure( "getBaseData: Can't retrieve skin data this way" );
			break;
		case SubmeshData::eVelocity:
			CU_Failure( "getBaseData: Can't retrieve velocity data this way" );
			break;
		case SubmeshData::eMeshlets:
			CU_Failure( "getBaseData: Can't retrieve meshlets data this way" );
			break;
		default:
			CU_Failure( "getBaseData: Unsupported SubmeshData type" );
			break;
		}

		static Point3fArray dummy{};
		return dummy;
	}

	GpuBufferOffsetT< Point4f > const & Submesh::getMorphTargets()const
	{
		if ( auto component = getComponent< MorphComponent >() )
		{
			return component->getData().getMorphTargets();
		}

		static GpuBufferOffsetT< Point4f > const dummy{};
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

	Vector< Meshlet > const & Submesh::getMeshlets()const
	{
		if ( auto component = getComponent< MeshletComponent >() )
		{
			return component->getData().getMeshletsData();
		}

		static Vector< Meshlet > const dummy{};
		return dummy;
	}

	Vector< Meshlet > & Submesh::getMeshlets()
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

	uint32_t Submesh::getIndexCount()const
	{
		if ( auto component = getIndexMapping() )
		{
			return component->getCount() * component->getComponentsCount();
		}

		return 0u;
	}

	bool Submesh::isDynamic()const
	{
		return ( hasComponent( SkinComponent::TypeName ) && getOwner()->getSkeleton() )
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
			return m_sourceBufferOffset;

		if ( auto it = m_finalBufferOffsets.find( geometry.getHash( pass, *this ) );
			it != m_finalBufferOffsets.end() )
			return it->second;

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
		return getFinalBufferOffsets( geometry, pass ).getFirstVertex< Point4f >();
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

	SubmeshComponentID Submesh::getComponentId( String const & componentType )const
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
			CU_Exception( "Submesh doesn't contain any render shader component" );
		}

		return m_render->getRenderData();
	}

	void Submesh::doInstantiate( Geometry const * geometry
		, Material const * oldMaterial
		, Material * newMaterial
		, bool update )
	{
		if ( newMaterial )
			newMaterial->initialise();

		if ( oldMaterial != newMaterial
			&& update
			&& m_instantiation )
		{
			auto & data = m_instantiation->getData();
			data.unref( oldMaterial );
			if ( data.ref( newMaterial ) )
				m_geometryBuffers.clear();
		}

		if ( newMaterial && geometry && isDynamic() )
		{
			for ( auto const & pass : *newMaterial )
			{
				if ( auto meshletComponent = getComponent< MeshletComponent >() )
					meshletComponent->getData().instantiate( *geometry, *pass );

				if ( auto it = m_finalBufferOffsets.try_emplace( geometry->getHash( *pass, *this ) ).first;
					m_initialised && !it->second )
				{
					// Initialise only if the submesh itself is already initialised,
					// because if it is not, the buffers will be initialised by the call to initialise().
					BufferBase const * indexBuffer{};
					if ( m_indexMapping )
						indexBuffer = &m_sourceBufferOffset.getBuffer( SubmeshData::eIndex );

					auto combine = getComponentCombine();
					auto const & engine = *getOwner()->getEngine();
					auto const & components = engine.getSubmeshComponentsRegister();
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

	void Submesh::doCreateDescriptorLayout( RenderDevice const & device )
	{
		ashes::VkDescriptorSetLayoutBindingArray bindings;
		auto combine = getComponentCombine();

		if ( combine.hasPositionFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePosition )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasNormalFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eNormal )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasTangentFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTangent )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasBitangentFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eBitangent )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasTexcoord0Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord0 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasTexcoord1Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord1 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasTexcoord2Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord2 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasTexcoord3Flag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eTexcoord3 )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasColourFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eColour )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasPassMaskFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::ePassMasks )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		if ( combine.hasVelocityFlag )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eVelocity )
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( MeshBuffersIdx::eInstances )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );

		m_descriptorLayout = device->createDescriptorSetLayout( smsh::getDescriptorName( *this )
			, c3d::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( smsh::getDescriptorName( *this )
			, MaxNodesPerPipeline );
	}

	String getPrefix( SubmeshContext const & context )
	{
		return getPrefix( *context.mesh );
	}

	Engine * getEngine( SubmeshContext const & context )
	{
		return getEngine( *context.mesh );
	}

	//*********************************************************************************************
}
