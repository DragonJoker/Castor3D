#include "CubeBoxManager.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>
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
			, PredefinedRgbColour const & colour )
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
			MaterialSPtr material;
			String matName = cuT( "BBox_" ) + getPredefinedName( colour );

			if ( !scene.getEngine()->getMaterialCache().has( matName ) )
			{
				material = scene.getEngine()->getMaterialCache().add( matName, scene.getMaterialsType() );
				auto pass = material->createPass();

				switch ( scene.getMaterialsType() )
				{
				case MaterialType::eLegacy:
					{
						std::static_pointer_cast< LegacyPass >( pass )->setDiffuse( RgbColour::fromPredefined( colour ) );
						std::static_pointer_cast< LegacyPass >( pass )->setAmbient( 1.0f );
					}
					break;

				case MaterialType::ePbrMetallicRoughness:
					{
						std::static_pointer_cast< MetallicRoughnessPbrPass >( pass )->setAlbedo( RgbColour::fromPredefined( colour ) );
					}
					break;

				case MaterialType::ePbrSpecularGlossiness:
					{
						std::static_pointer_cast< SpecularGlossinessPbrPass >( pass )->setDiffuse( RgbColour::fromPredefined( colour ) );
					}
					break;
				}
			}
			else
			{
				material = scene.getEngine()->getMaterialCache().find( matName );
			}

			submesh->setDefaultMaterial( material );
			result->computeContainers();
			scene.getListener().postEvent( makeInitialiseEvent( *submesh ) );
			return result;
		}
	}

	CubeBoxManager::CubeBoxManager( Scene & scene )
		: m_scene{ scene }
		, m_obbMesh{ doCreateCubeMesh( cuT( "CubeBox_OBB" ), scene, PredefinedRgbColour::eRed ) }
		, m_obbSubmesh{ doCreateCubeMesh( cuT( "CubeBox_OBB_Submesh" ), scene, PredefinedRgbColour::eBlue ) }
		, m_aabbMesh{ doCreateCubeMesh( cuT( "CubeBox_AABB" ), scene, PredefinedRgbColour::eGreen ) }
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
				m_obbNode = doAddBB( m_obbMesh
					, m_obbMesh->getName()
					, object.getParent()
					, m_objectMesh->getBoundingBox() );
				m_aabbNode = doAddBB( m_aabbMesh
					, m_aabbMesh->getName()
					, m_scene.getObjectRootNode()
					, m_objectMesh->getBoundingBox() );

				for ( auto & submesh : *m_objectMesh )
				{
					m_obbSubmeshNodes.push_back( doAddBB( m_obbSubmesh
						, m_obbMesh->getName() + string::toString( submesh->getId() )
						, object.getParent()
						, submesh->getBoundingBox() ) );
				}

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
				doRemoveBB( m_obbMesh->getName(), m_obbNode );
				doRemoveBB( m_aabbMesh->getName(), m_aabbNode );
				uint32_t i = 0u;

				for ( auto & submesh : *m_objectMesh )
				{
					doRemoveBB( m_obbMesh->getName() + string::toString( submesh->getId() )
						, m_obbSubmeshNodes[i] );
					i++;
				}

				m_obbSubmeshNodes.clear();
				m_obbNode.reset();
				m_aabbNode.reset();
				m_objectMesh = nullptr;
				m_objectName.clear();
			} ) );
	}

	SceneNodeSPtr CubeBoxManager::doAddBB( MeshSPtr bbMesh
		, String const & nameSpec
		, SceneNodeSPtr parent
		, BoundingBox const & bb )
	{
		auto name = m_objectName + cuT( "-" ) + nameSpec;
		auto node = m_scene.getSceneNodeCache().add( name, parent );
		auto geometry = m_scene.getGeometryCache().add( name, node, bbMesh );
		geometry->setShadowCaster( false );

		if ( parent != m_scene.getObjectRootNode() )
		{
			auto scale = ( bb.getMax() - bb.getMin() ) / 2.0_r;
			node->setScale( scale );
			node->setPosition( bb.getCenter() );
			node->setVisible( true );
		}

		for ( auto & submesh : *bbMesh )
		{
			geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
		}

		return node;
	}

	void CubeBoxManager::doRemoveBB( String const & nameSpec
		, SceneNodeSPtr bbNode )
	{
		auto name = m_objectName + cuT( "-" ) + nameSpec;

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
		m_obbNode->setPosition( obb.getCenter() );
		m_obbNode->update();
		uint32_t i = 0u;

		for ( auto & submesh : *m_objectMesh )
		{
			auto & sobb = submesh->getBoundingBox();
			m_obbSubmeshNodes[i]->setScale( ( sobb.getMax() - sobb.getMin() ) / 2.0_r );
			m_obbSubmeshNodes[i]->setPosition( sobb.getCenter() );
			m_obbSubmeshNodes[i]->update();
			++i;
		}

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
