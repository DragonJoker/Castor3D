#include "CubeBoxManager.hpp"

#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/SubmeshComponent/LinesMapping.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>

#include <cstddef>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	CubeBoxManager::CubeBoxManager( Scene & scene )
		: m_scene{ scene }
	{
		m_mesh = m_scene.getMeshCache().add( cuT( "CubeManager_Cube" ) );
		auto submesh = m_mesh->createSubmesh();
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
		submesh->setDefaultMaterial( m_scene.getEngine()->getMaterialCache().find( cuT( "Red" ) ) );
		m_mesh->computeContainers();
		m_scene.getListener().postEvent( makeInitialiseEvent( *submesh ) );
	}

	CubeBoxManager::~CubeBoxManager()
	{
		m_mesh.reset();
		m_scene.getMeshCache().remove( cuT( "CubeManager_Cube" ) );
	}

	void CubeBoxManager::displayObject( Geometry const & object )
	{
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				auto mesh = object.getMesh();
				auto node = m_scene.getSceneNodeCache().add( object.getName() + cuT( "-CubeNode" ), object.getParent() );
				auto geometry = m_scene.getGeometryCache().add( object.getName() + cuT( "-Cube" ), node, m_mesh );
				auto scale = ( mesh->getCollisionBox().getMax() - mesh->getCollisionBox().getMin() ) / 2;
				node->setScale( scale );
				node->setPosition( mesh->getCollisionBox().getCenter() );
				node->setVisible( true );

				for ( auto & submesh : *m_mesh )
				{
					geometry->setMaterial( *submesh, m_scene.getEngine()->getMaterialCache().find( cuT( "Red" ) ) );
				}
			} ) );
	}

	void CubeBoxManager::hideObject( Geometry const & object )
	{
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				if ( m_scene.getGeometryCache().has( object.getName() + cuT( "-Cube" ) ) )
				{
					m_scene.getGeometryCache().remove( object.getName() + cuT( "-Cube" ) );
				}

				if ( m_scene.getSceneNodeCache().has( object.getName() + cuT( "-CubeNode" ) ) )
				{
					auto node = m_scene.getSceneNodeCache().find( object.getName() + cuT( "-CubeNode" ) );
					node->setVisible( false );
					node.reset();
					m_scene.getSceneNodeCache().remove( object.getName() + cuT( "-CubeNode" ) );
				}
			} ) );
	}
}
