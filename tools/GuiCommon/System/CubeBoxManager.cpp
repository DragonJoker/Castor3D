#include "GuiCommon/System/CubeBoxManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

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
				InterleavedVertex{}.position( Point3f{ -1, -1, -1 } ),
				InterleavedVertex{}.position( Point3f{ -1, +1, -1 } ),
				InterleavedVertex{}.position( Point3f{ +1, +1, -1 } ),
				InterleavedVertex{}.position( Point3f{ +1, -1, -1 } ),
				InterleavedVertex{}.position( Point3f{ -1, -1, +1 } ),
				InterleavedVertex{}.position( Point3f{ -1, +1, +1 } ),
				InterleavedVertex{}.position( Point3f{ +1, +1, +1 } ),
				InterleavedVertex{}.position( Point3f{ +1, -1, +1 } ),
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
				material = scene.getEngine()->getMaterialCache().add( matName, scene.getPassesType() );
				auto pass = material->createPass();
				pass->enableLighting( false );
				pass->enablePicking( false );
				pass->setColour( colour );
			}
			else
			{
				material = scene.getEngine()->getMaterialCache().find( matName );
			}

			submesh->setDefaultMaterial( material );
			result->computeContainers();
			scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
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
		engine->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &object, &submesh]( RenderDevice const & device
				, QueueData const & queueData )
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
		engine->postEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &object]( RenderDevice const & device
				, QueueData const & queueData )
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
		SceneNodeSPtr result;
		auto name = m_object->getName() + cuT( "-" ) + nameSpec;

		if ( !m_scene.getSceneNodeCache().has( name ) )
		{
			result = m_scene.getSceneNodeCache().add( name, parent );
		}
		else
		{
			result = m_scene.getSceneNodeCache().find( name );
			result->attachTo( parent );
		}

		GeometrySPtr geometry;

		if ( !m_scene.getGeometryCache().has( name ) )
		{
			geometry = m_scene.getGeometryCache().add( name, *result, bbMesh );
		}
		else
		{
			geometry = m_scene.getGeometryCache().find( name );
		}

		geometry->setShadowCaster( false );

		if ( &parent != m_scene.getObjectRootNode().get() )
		{
			auto scale = ( bb.getMax() - bb.getMin() ) / 2.0f;
			result->setScale( scale );
			result->setPosition( bb.getCenter() );
			result->setVisible( true );
		}

		for ( auto & submesh : *geometry->getMesh() )
		{
			geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
		}

		return result;
	}

	void CubeBoxManager::doRemoveBB( String const & nameSpec
		, SceneNodeSPtr bbNode )
	{
		bbNode->setVisible( false );
	}

	void CubeBoxManager::onSceneUpdate( castor3d::Scene const & scene )
	{
		if ( m_obbNode->isVisible() && m_obbNode->isDisplayable() )
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
			engine->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, aabbSubmesh]( RenderDevice const & device
					, QueueData const & queueData )
				{
					aabbSubmesh->update();
				} ) );
		}
	}
}
