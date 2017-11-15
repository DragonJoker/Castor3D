#include "CubeBoxManager.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/SubmeshComponent/LinesMapping.hpp>
#include <Mesh/Vertex.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

#include <cstddef>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	namespace
	{
		MeshSPtr doCreateCubeMesh( String const name
			, Scene & scene
			, String const & material )
		{
			auto result = scene.getMeshCache().add( name );
			auto submesh = result->createSubmesh();
			InterleavedVertexArray vertex
			{
				InterleavedVertex{ { -1, -1, -1 } },
				InterleavedVertex{ { -1, +1, -1 } },
				InterleavedVertex{ { +1, +1, -1 } },
				InterleavedVertex{ { +1, -1, -1 } },
				InterleavedVertex{ { -1, -1, +1 } },
				InterleavedVertex{ { -1, +1, +1 } },
				InterleavedVertex{ { +1, +1, +1 } },
				InterleavedVertex{ { +1, -1, +1 } },
			};
			submesh->setTopology( Topology::eLines );
			submesh->addPoints( vertex );
			auto mapping = std::make_shared< LinesMapping >( *submesh );
			LineIndices lines[]
			{
				LineIndices{ { 0u, 1u } },
				LineIndices{ { 1u, 2u } },
				LineIndices{ { 2u, 3u } },
				LineIndices{ { 3u, 0u } },
				LineIndices{ { 4u, 5u } },
				LineIndices{ { 5u, 6u } },
				LineIndices{ { 6u, 7u } },
				LineIndices{ { 7u, 4u } },
				LineIndices{ { 0u, 4u } },
				LineIndices{ { 1u, 5u } },
				LineIndices{ { 2u, 6u } },
				LineIndices{ { 3u, 7u } },
			};
			mapping->addLineGroup( lines );
			submesh->setIndexMapping( mapping );
			submesh->setDefaultMaterial( scene.getEngine()->getMaterialCache().find( material ) );
			result->computeContainers();
			scene.getListener().postEvent( makeInitialiseEvent( *submesh ) );
			return result;
		}
	}

	CubeBoxManager::CubeBoxManager( Scene & scene )
		: m_scene{ scene }
		, m_OBBMesh{ doCreateCubeMesh( cuT( "CubeBox_OBB" ), scene, cuT( "Red" ) ) }
		, m_AABBMesh{ doCreateCubeMesh( cuT( "CubeBox_AABB" ), scene, cuT( "Green" ) ) }
	{
	}

	CubeBoxManager::~CubeBoxManager()
	{
		if ( m_objectMesh )
		{
			m_connection.disconnect();
			m_objectMesh = nullptr;
			m_objectName.clear();
		}

		m_AABBMesh.reset();
		m_OBBMesh.reset();
	}

	void CubeBoxManager::displayObject( Geometry const & object )
	{
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				m_objectMesh = object.getMesh();
				m_objectName = object.getName();
				doAddBB( m_OBBMesh, object.getParent() );
				doAddBB( m_AABBMesh, m_scene.getObjectRootNode() );
				m_connection = object.getParent()->onChanged.connect( std::bind( &CubeBoxManager::onNodeChanged
					, this
					, std::placeholders::_1 ) );
				onNodeChanged( *object.getParent() );
			} ) );
	}

	void CubeBoxManager::hideObject( Geometry const & object )
	{
		REQUIRE( object.getName() == m_objectName );
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				m_connection.disconnect();
				doRemoveBB( m_OBBMesh );
				doRemoveBB( m_AABBMesh );
				m_objectMesh = nullptr;
				m_objectName.clear();
			} ) );
	}

	void CubeBoxManager::doAddBB( MeshSPtr bbMesh
		, SceneNodeSPtr parent )
	{
		auto name = m_objectName + cuT( "-" ) + bbMesh->getName();
		auto node = m_scene.getSceneNodeCache().add( name, parent );
		auto geometry = m_scene.getGeometryCache().add( name, node, bbMesh );
		geometry->setShadowCaster( false );

		if ( parent != m_scene.getObjectRootNode() )
		{
			auto scale = ( m_objectMesh->getBoundingBox().getMax() - m_objectMesh->getBoundingBox().getMin() ) / 2.0_r;
			node->setScale( scale );
			node->setPosition( m_objectMesh->getBoundingBox().getCenter() );
			node->setVisible( true );
		}

		for ( auto & submesh : *bbMesh )
		{
			geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
		}
	}

	void CubeBoxManager::doRemoveBB( castor3d::MeshSPtr bbMesh )
	{
		auto name = m_objectName + cuT( "-" ) + bbMesh->getName();

		if ( m_scene.getGeometryCache().has( name ) )
		{
			m_scene.getGeometryCache().remove( name );
		}

		if ( m_scene.getSceneNodeCache().has( name ) )
		{
			auto node = m_scene.getSceneNodeCache().find( name );
			node->setVisible( false );
			node.reset();
			m_scene.getSceneNodeCache().remove( name );
		}
	}

	void CubeBoxManager::onNodeChanged( castor3d::SceneNode const & node )
	{
		auto aabb = m_objectMesh->getBoundingBox().getAxisAligned( node.getDerivedTransformationMatrix() );

		// Update submesh
		auto & min = aabb.getMin();
		auto & max = aabb.getMax();
		auto submesh = m_AABBMesh->getSubmesh( 0u );
		Vertex::setPosition( *submesh->getPoint( 0u ), min[0], min[1], min[2] );
		Vertex::setPosition( *submesh->getPoint( 1u ), min[0], max[1], min[2] );
		Vertex::setPosition( *submesh->getPoint( 2u ), max[0], max[1], min[2] );
		Vertex::setPosition( *submesh->getPoint( 3u ), max[0], min[1], min[2] );
		Vertex::setPosition( *submesh->getPoint( 4u ), min[0], min[1], max[2] );
		Vertex::setPosition( *submesh->getPoint( 5u ), min[0], max[1], max[2] );
		Vertex::setPosition( *submesh->getPoint( 6u ), max[0], max[1], max[2] );
		Vertex::setPosition( *submesh->getPoint( 7u ), max[0], min[1], max[2] );
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this, submesh]()
			{
				submesh->getVertexBuffer().upload();
			} ) );
	}
}
