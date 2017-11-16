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
		, m_obbMesh{ doCreateCubeMesh( cuT( "CubeBox_OBB" ), scene, cuT( "Red" ) ) }
		, m_aabbMesh{ doCreateCubeMesh( cuT( "CubeBox_AABB" ), scene, cuT( "Green" ) ) }
	{
	}

	CubeBoxManager::~CubeBoxManager()
	{
		if ( m_objectMesh )
		{
			m_sceneConnection.disconnect();
			m_objectMesh = nullptr;
			m_objectName.clear();
		}

		m_aabbMesh.reset();
		m_obbMesh.reset();
		m_aabbNode.reset();
		m_obbNode.reset();
	}

	void CubeBoxManager::displayObject( Geometry const & object )
	{
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				m_objectMesh = object.getMesh();
				m_objectName = object.getName();
				m_obbNode = doAddBB( m_obbMesh, object.getParent() );
				m_aabbNode = doAddBB( m_aabbMesh, m_scene.getObjectRootNode() );
				m_sceneConnection = m_scene.onUpdate.connect( std::bind( &CubeBoxManager::onSceneUpdate
					, this
					, std::placeholders::_1 ) );
			} ) );
	}

	void CubeBoxManager::hideObject( Geometry const & object )
	{
		REQUIRE( object.getName() == m_objectName );
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				m_sceneConnection.disconnect();
				doRemoveBB( m_obbMesh, m_obbNode );
				doRemoveBB( m_aabbMesh, m_aabbNode );
				m_obbNode.reset();
				m_aabbNode.reset();
				m_objectMesh = nullptr;
				m_objectName.clear();
			} ) );
	}

	SceneNodeSPtr CubeBoxManager::doAddBB( MeshSPtr bbMesh
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

		return node;
	}

	void CubeBoxManager::doRemoveBB( MeshSPtr bbMesh
		, SceneNodeSPtr bbNode )
	{
		auto name = m_objectName + cuT( "-" ) + bbMesh->getName();

		if ( m_scene.getGeometryCache().has( name ) )
		{
			m_scene.getGeometryCache().remove( name );
		}

		m_scene.getSceneNodeCache().remove( bbNode->getName() );

		bbNode->setVisible( false );
	}

	void CubeBoxManager::onSceneUpdate( castor3d::Scene const & scene )
	{
		auto & obb = m_objectMesh->getBoundingBox();
		auto scale = ( obb.getMax() - obb.getMin() ) / 2.0_r;
		m_obbNode->setScale( scale );
		m_obbNode->setPosition( m_objectMesh->getBoundingBox().getCenter() );
		m_obbNode->update();

		auto aabb = obb.getAxisAligned( m_obbNode->getParent()->getDerivedTransformationMatrix() );
		auto & aabbMin = aabb.getMin();
		auto & aabbMax = aabb.getMax();
		auto aabbSubmesh = m_aabbMesh->getSubmesh( 0u );
		Vertex::setPosition( *aabbSubmesh->getPoint( 0u ), aabbMin[0], aabbMin[1], aabbMin[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 1u ), aabbMin[0], aabbMax[1], aabbMin[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 2u ), aabbMax[0], aabbMax[1], aabbMin[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 3u ), aabbMax[0], aabbMin[1], aabbMin[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 4u ), aabbMin[0], aabbMin[1], aabbMax[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 5u ), aabbMin[0], aabbMax[1], aabbMax[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 6u ), aabbMax[0], aabbMax[1], aabbMax[2] );
		Vertex::setPosition( *aabbSubmesh->getPoint( 7u ), aabbMax[0], aabbMin[1], aabbMax[2] );

		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this, aabbSubmesh]()
			{
				aabbSubmesh->getVertexBuffer().upload();
			} ) );
	}
}
