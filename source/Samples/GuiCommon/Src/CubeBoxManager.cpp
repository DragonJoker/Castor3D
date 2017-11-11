#include "CubeBoxManager.hpp"

#include <cstddef>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	CubeBoxManager::CubeBoxManager( Scene & scene )
		: m_scene{ scene }
	{
		Parameters parameters;
		parameters.add( cuT( "width" ), cuT( "1.0" ) );
		parameters.add( cuT( "height" ), cuT( "1.0" ) );
		parameters.add( cuT( "depth" ), cuT( "1.0" ) );
		m_mesh = m_scene.getMeshCache().add( cuT( "CubeManager_Cube" ) );
		m_scene.getEngine()->getMeshFactory().create( "cube" )->generate( *m_mesh, parameters );
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
				auto node = m_scene.getSceneNodeCache().add( object.getName() + cuT( "-CubeNode" ), object.getParent() );
				auto geometry = m_scene.getGeometryCache().add( object.getName() + cuT( "-Cube" ), node, m_mesh );
				auto scale = m_mesh->getCollisionBox().getMax() - m_mesh->getCollisionBox().getMin();
				node->setScale( scale );
				node->setVisible( true );

				for ( auto & submesh : *m_mesh )
				{
					geometry->setMaterial( *submesh, m_scene.getEngine()->getMaterialCache().find( cuT( "Red" ) ) );
				}

				for ( auto & submesh : *geometry->getMesh() )
				{
					submesh->setTopology( Topology::eLineStrip );
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
