#include "GuiCommon/System/CubeBoxManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <cstddef>

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
	namespace
	{
		MeshSPtr doCreateCubeMesh( String const name
			, Scene & scene
			, RgbColour const & colour
			, String const & colourName )
		{
			auto result = scene.getMeshCache().add( name );
			result->setSerialisable( false );
			auto submesh = result->createSubmesh();
			static InterleavedVertexArray const vertex
			{
				InterleavedVertex::createP( Point3f{ -1, -1, -1 } ),
				InterleavedVertex::createP( Point3f{ -1, +1, -1 } ),
				InterleavedVertex::createP( Point3f{ +1, +1, -1 } ),
				InterleavedVertex::createP( Point3f{ +1, -1, -1 } ),
				InterleavedVertex::createP( Point3f{ -1, -1, +1 } ),
				InterleavedVertex::createP( Point3f{ -1, +1, +1 } ),
				InterleavedVertex::createP( Point3f{ +1, +1, +1 } ),
				InterleavedVertex::createP( Point3f{ +1, -1, +1 } ),
			};
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
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
			String matName = cuT( "BBox_" ) + colourName;

			if ( !scene.getEngine()->getMaterialCache().has( matName ) )
			{
				material = scene.getEngine()->getMaterialCache().add( matName, scene.getMaterialsType() );
				auto pass = material->createPass();

				switch ( scene.getMaterialsType() )
				{
				case MaterialType::ePhong:
					{
						auto & phong = *std::static_pointer_cast< PhongPass >( pass );
						phong.setDiffuse( colour );
						phong.setSpecular( colour );
						phong.setAmbient( 1.0f );
					}
					break;

				case MaterialType::eMetallicRoughness:
					{
						auto & pbrmr = *std::static_pointer_cast< MetallicRoughnessPbrPass >( pass );
						pbrmr.setAlbedo( colour );
						pbrmr.setRoughness( 1.0f );
						pbrmr.setMetallic( 0.0f );
						pbrmr.setEmissive( 1.0f );
					}
					break;

				case MaterialType::eSpecularGlossiness:
					{
						auto & pbrsg = *std::static_pointer_cast< SpecularGlossinessPbrPass >( pass );
						pbrsg.setDiffuse( colour );
						pbrsg.setSpecular( colour );
						pbrsg.setGlossiness( 0.0f );
						pbrsg.setEmissive( 1.0f );
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
		, m_obbMesh{ doCreateCubeMesh( cuT( "CubeBox_OBB" ), scene, RgbColour::fromPredefined( PredefinedRgbColour::eRed ), cuT( "Red" ) ) }
		, m_obbSubmesh{ doCreateCubeMesh( cuT( "CubeBox_OBB_Submesh" ), scene, RgbColour::fromPredefined( PredefinedRgbColour::eBlue ), cuT( "Blue" ) ) }
		, m_obbSelectedSubmesh{ doCreateCubeMesh( cuT( "CubeBox_OBB_SelectedSubmesh" ), scene, RgbColour::fromComponents( 1.0f, 1.0f, 0.0f ), cuT( "Yellow" ) ) }
		, m_obbBone{ doCreateCubeMesh( cuT( "CubeBox_OBB_Bone" ), scene, RgbColour::fromPredefined( PredefinedRgbColour::eDarkBlue ), cuT( "DarkBlue" ) ) }
		, m_aabbMesh{ doCreateCubeMesh( cuT( "CubeBox_AABB" ), scene, RgbColour::fromPredefined( PredefinedRgbColour::eGreen ), cuT( "Green" ) ) }
	{
	}

	CubeBoxManager::~CubeBoxManager()
	{
		if ( m_object )
		{
			m_sceneConnection.disconnect();
			m_object = nullptr;
		}

		m_aabbMesh.reset();
		m_obbMesh.reset();
		m_obbBone.reset();
		m_obbSelectedSubmesh.reset();
		m_obbSubmesh.reset();
		m_aabbNode.reset();
		m_obbNode.reset();
	}

	void CubeBoxManager::displayObject( Geometry const & object
		, castor3d::Submesh const & submesh )
	{
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object, &submesh]()
			{
				m_object = &object;
				m_submesh = &submesh;
				m_obbNode = doAddBB( m_obbMesh
					, m_obbMesh->getName()
					, *object.getParent()
					, m_object->getBoundingBox() );
				m_aabbNode = doAddBB( m_aabbMesh
					, m_aabbMesh->getName()
					, *m_scene.getObjectRootNode()
					, m_object->getBoundingBox() );
				m_obbSelectedSubmeshNode = doAddBB( m_obbSelectedSubmesh
					, m_obbMesh->getName() + string::toString( submesh.getId() )
					, *object.getParent()
					, m_object->getBoundingBox( submesh ) );

				for ( auto & submesh : *m_object->getMesh() )
				{
					if ( submesh.get() != m_submesh )
					{
						m_obbSubmeshNodes.push_back( doAddBB( m_obbSubmesh
							, m_obbMesh->getName() + string::toString( submesh->getId() )
							, *object.getParent()
							, m_object->getBoundingBox( *submesh ) ) );
					}
				}

				m_sceneConnection = m_scene.onUpdate.connect( [this]( Scene const & scene )
					{
						onSceneUpdate( scene );
					} );
			} ) );
	}

	void CubeBoxManager::hideObject( Geometry const & object )
	{
		CU_Require( object.getName() == m_object->getName() );
		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePostRender
			, [this, &object]()
			{
				m_sceneConnection.disconnect();
				doRemoveBB( m_obbMesh->getName(), m_obbNode );
				doRemoveBB( m_aabbMesh->getName(), m_aabbNode );
				doRemoveBB( m_obbSelectedSubmesh->getName(), m_obbSelectedSubmeshNode );
				uint32_t i = 0u;

				for ( auto & submesh : *m_object->getMesh() )
				{
					if ( submesh.get() != m_submesh )
					{
						doRemoveBB( m_obbMesh->getName() + string::toString( submesh->getId() )
							, m_obbSubmeshNodes[i] );
						i++;
					}
				}

				i = 0u;

				for ( auto & node : m_obbBoneNodes )
				{
					doRemoveBB( m_obbMesh->getName() + cuT( "_Bone_" ) + string::toString( i++ )
						, node );
				}

				m_obbBoneNodes.clear();
				m_obbSubmeshNodes.clear();
				m_obbSelectedSubmeshNode = nullptr;
				m_obbNode.reset();
				m_aabbNode.reset();
				m_object = nullptr;
				m_submesh = nullptr;
			} ) );
	}

	SceneNodeSPtr CubeBoxManager::doAddBB( MeshSPtr bbMesh
		, String const & nameSpec
		, SceneNode & parent
		, BoundingBox const & bb )
	{
		auto name = m_object->getName() + cuT( "-" ) + nameSpec;
		auto node = m_scene.getSceneNodeCache().add( name, parent );
		auto geometry = m_scene.getGeometryCache().add( name, *node, bbMesh );
		geometry->setShadowCaster( false );

		if ( &parent != m_scene.getObjectRootNode().get() )
		{
			auto scale = ( bb.getMax() - bb.getMin() ) / 2.0f;
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
		auto name = m_object->getName() + cuT( "-" ) + nameSpec;

		if ( m_scene.getGeometryCache().has( name ) )
		{
			m_scene.getGeometryCache().remove( name );
		}

		m_scene.getSceneNodeCache().remove( bbNode->getName() );

		bbNode->setVisible( false );
	}

	void CubeBoxManager::onSceneUpdate( castor3d::Scene const & scene )
	{
		auto & obb = m_object->getBoundingBox();
		auto scale = obb.getDimensions() / 2.0f;
		m_obbNode->setScale( scale );
		m_obbNode->setPosition( obb.getCenter() );
		m_obbNode->update();
		uint32_t i = 0u;
		auto & sobb = m_object->getBoundingBox( *m_submesh );
		m_obbSelectedSubmeshNode->setScale( sobb.getDimensions() / 2.0f );
		m_obbSelectedSubmeshNode->setPosition( sobb.getCenter() );
		m_obbSelectedSubmeshNode->update();

		for ( auto & submesh : *m_object->getMesh() )
		{
			if ( submesh.get() != m_submesh )
			{
				auto & sobb = m_object->getBoundingBox( *submesh );
				m_obbSubmeshNodes[i]->setScale( sobb.getDimensions() / 2.0f );
				m_obbSubmeshNodes[i]->setPosition( sobb.getCenter() );
				m_obbSubmeshNodes[i]->update();
				++i;
			}
		}

		auto aabb = obb.getAxisAligned( m_obbNode->getParent()->getDerivedTransformationMatrix() );
		auto aabbMin = aabb.getMin();
		auto aabbMax = aabb.getMax();
		auto aabbSubmesh = m_aabbMesh->getSubmesh( 0u );
		aabbSubmesh->getPoint( 0u ).pos = Point3f( aabbMin[0], aabbMin[1], aabbMin[2] );
		aabbSubmesh->getPoint( 1u ).pos = Point3f( aabbMin[0], aabbMax[1], aabbMin[2] );
		aabbSubmesh->getPoint( 2u ).pos = Point3f( aabbMax[0], aabbMax[1], aabbMin[2] );
		aabbSubmesh->getPoint( 3u ).pos = Point3f( aabbMax[0], aabbMin[1], aabbMin[2] );
		aabbSubmesh->getPoint( 4u ).pos = Point3f( aabbMin[0], aabbMin[1], aabbMax[2] );
		aabbSubmesh->getPoint( 5u ).pos = Point3f( aabbMin[0], aabbMax[1], aabbMax[2] );
		aabbSubmesh->getPoint( 6u ).pos = Point3f( aabbMax[0], aabbMax[1], aabbMax[2] );
		aabbSubmesh->getPoint( 7u ).pos = Point3f( aabbMax[0], aabbMin[1], aabbMax[2] );
		aabbSubmesh->needsUpdate();

		Engine * engine = m_scene.getEngine();
		engine->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this, aabbSubmesh]()
			{
				aabbSubmesh->update();
			} ) );
	}
}
