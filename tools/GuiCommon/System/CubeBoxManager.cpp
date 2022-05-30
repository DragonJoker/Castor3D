#include "GuiCommon/System/CubeBoxManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <cstddef>

namespace GuiCommon
{
	namespace
	{
		castor3d::MeshResPtr doCreateCubeMesh( castor::String const name
			, castor3d::Scene & scene
			, castor::RgbColour const & colour
			, castor::String const & colourName )
		{
			auto result = scene.addNewMesh( name, scene );
			result.lock()->setSerialisable( false );
			auto submesh = result.lock()->createSubmesh();
			static castor::Point4fArray const vertex
			{
				castor::Point4f{ -1, -1, -1, 0 },
				castor::Point4f{ -1, +1, -1, 0 },
				castor::Point4f{ +1, +1, -1, 0 },
				castor::Point4f{ +1, -1, -1, 0 },
				castor::Point4f{ -1, -1, +1, 0 },
				castor::Point4f{ -1, +1, +1, 0 },
				castor::Point4f{ +1, +1, +1, 0 },
				castor::Point4f{ +1, -1, +1, 0 },
			};
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
			auto positions = submesh->createComponent< castor3d::PositionsComponent >();
			positions->getData() = vertex;
			auto mapping = submesh->createComponent< castor3d::LinesMapping >();
			castor3d::LineIndices lines[]
			{
				castor3d::LineIndices{ { 0u, 1u } },
				castor3d::LineIndices{ { 1u, 2u } },
				castor3d::LineIndices{ { 2u, 3u } },
				castor3d::LineIndices{ { 3u, 0u } },
				castor3d::LineIndices{ { 4u, 5u } },
				castor3d::LineIndices{ { 5u, 6u } },
				castor3d::LineIndices{ { 6u, 7u } },
				castor3d::LineIndices{ { 7u, 4u } },
				castor3d::LineIndices{ { 0u, 4u } },
				castor3d::LineIndices{ { 1u, 5u } },
				castor3d::LineIndices{ { 2u, 6u } },
				castor3d::LineIndices{ { 3u, 7u } },
			};
			mapping->addLineGroup( lines );
			castor3d::MaterialResPtr material;
			castor::String matName = cuT( "BBox_" ) + colourName;

			if ( !scene.getEngine()->hasMaterial( matName ) )
			{
				material = scene.getEngine()->addNewMaterial( matName
					, *scene.getEngine()
					, scene.getPassesType() );
				auto pass = material.lock()->createPass();
				pass->enableLighting( false );
				pass->enablePicking( false );
				pass->setColour( colour );
			}
			else
			{
				material = scene.getEngine()->findMaterial( matName );
			}

			submesh->setDefaultMaterial( material.lock().get() );
			result.lock()->computeContainers();
			scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			return result;
		}
	}

	CubeBoxManager::CubeBoxManager( castor3d::Scene & scene )
		: m_scene{ scene }
		, m_obbMesh{ doCreateCubeMesh( cuT( "CubeBox_OBB" ), scene, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eRed ), cuT( "Red" ) ) }
		, m_obbSubmesh{ doCreateCubeMesh( cuT( "CubeBox_OBB_Submesh" ), scene, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eBlue ), cuT( "Blue" ) ) }
		, m_obbSelectedSubmesh{ doCreateCubeMesh( cuT( "CubeBox_OBB_SelectedSubmesh" ), scene, castor::RgbColour::fromComponents( 1.0f, 1.0f, 0.0f ), cuT( "Yellow" ) ) }
		, m_obbBone{ doCreateCubeMesh( cuT( "CubeBox_OBB_Bone" ), scene, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eDarkBlue ), cuT( "DarkBlue" ) ) }
		, m_aabbMesh{ doCreateCubeMesh( cuT( "CubeBox_AABB" ), scene, castor::RgbColour::fromPredefined( castor::PredefinedRgbColour::eGreen ), cuT( "Green" ) ) }
	{
	}

	CubeBoxManager::~CubeBoxManager()
	{
		if ( m_object )
		{
			m_sceneConnection.disconnect();
			m_object = nullptr;
		}

		m_aabbMesh = {};
		m_obbMesh = {};
		m_obbBone = {};
		m_obbSelectedSubmesh = {};
		m_obbSubmesh = {};
		m_aabbNode.reset();
		m_obbNode.reset();
	}

	void CubeBoxManager::displayObject( castor3d::Geometry const & object
		, castor3d::Submesh const & submesh )
	{
		castor3d::Engine * engine = m_scene.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
			, [this, &object, &submesh]()
			{
				m_object = &object;
				m_submesh = &submesh;
				m_aabbNode = doAddBB( m_aabbMesh
					, m_aabbMesh.lock()->getName()
					, *m_scene.getObjectRootNode()
					, m_object->getBoundingBox() );
				m_aabbNode->setScale( { 1.0f, 1.0f, 1.0f } );
				m_aabbNode->setPosition( {} );
				m_obbNode = doAddBB( m_obbMesh
					, m_obbMesh.lock()->getName()
					, *object.getParent()
					, m_object->getBoundingBox() );
				m_obbSelectedSubmeshNode = doAddBB( m_obbSelectedSubmesh
					, m_obbMesh.lock()->getName() + castor::string::toString( submesh.getId() )
					, *object.getParent()
					, m_object->getBoundingBox( submesh ) );

				for ( auto & psubmesh : *m_object->getMesh().lock() )
				{
					if ( psubmesh.get() != m_submesh )
					{
						m_obbSubmeshNodes.push_back( doAddBB( m_obbSubmesh
							, m_obbMesh.lock()->getName() + castor::string::toString( psubmesh->getId() )
							, *object.getParent()
							, m_object->getBoundingBox( *psubmesh ) ) );
					}
				}

				m_sceneConnection = m_scene.onUpdate.connect( [this]( castor3d::Scene const & scene )
					{
						onSceneUpdate( scene );
					} );
			} ) );
	}

	void CubeBoxManager::hideObject( castor3d::Geometry const & object )
	{
		CU_Require( object.getName() == m_object->getName() );
		castor3d::Engine * engine = m_scene.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
			, [this]()
			{
				m_sceneConnection.disconnect();
				doRemoveBB( m_aabbMesh.lock()->getName(), m_aabbNode );
				doRemoveBB( m_obbMesh.lock()->getName(), m_obbNode );
				doRemoveBB( m_obbSelectedSubmesh.lock()->getName(), m_obbSelectedSubmeshNode );
				uint32_t i = 0u;

				for ( auto & submesh : *m_object->getMesh().lock() )
				{
					if ( submesh.get() != m_submesh )
					{
						doRemoveBB( m_obbMesh.lock()->getName() + castor::string::toString( submesh->getId() )
							, m_obbSubmeshNodes[i] );
						i++;
					}
				}

				i = 0u;

				for ( auto & node : m_obbBoneNodes )
				{
					doRemoveBB( m_obbMesh.lock()->getName() + cuT( "_Bone_" ) + castor::string::toString( i++ )
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

	castor3d::SceneNodeSPtr CubeBoxManager::doAddBB( castor3d::MeshResPtr bbMesh
		, castor::String const & nameSpec
		, castor3d::SceneNode & parent
		, castor::BoundingBox const & bb )
	{
		castor3d::SceneNodeSPtr result;
		auto name = m_object->getName() + cuT( "-" ) + nameSpec;

		if ( !m_scene.hasSceneNode( name ) )
		{
			result = m_scene.addNewSceneNode( name ).lock();
			result->setSerialisable( false );
		}
		else
		{
			result = m_scene.findSceneNode( name ).lock();
		}

		castor3d::GeometrySPtr ownGeometry;
		castor3d::GeometrySPtr geometry;

		if ( !m_scene.hasGeometry( name ) )
		{
			ownGeometry = std::make_shared< castor3d::Geometry >( name, m_scene, *result, bbMesh );
			geometry = ownGeometry;
		}
		else
		{
			geometry = m_scene.findGeometry( name ).lock();
		}

		geometry->setShadowCaster( false );
		auto scale = ( bb.getDimensions() ) / 2.0f;
		result->attachTo( parent );
		result->setScale( scale );
		result->setPosition( bb.getCenter() );
		result->setVisible( true );

		for ( auto & submesh : *geometry->getMesh().lock() )
		{
			geometry->setMaterial( *submesh, submesh->getDefaultMaterial() );
		}

		if ( ownGeometry )
		{
			m_scene.addGeometry( std::move( ownGeometry ) );
		}

		return result;
	}

	void CubeBoxManager::doRemoveBB( castor::String const & nameSpec
		, castor3d::SceneNodeSPtr bbNode )
	{
		bbNode->setVisible( false );
	}

	void CubeBoxManager::onSceneUpdate( castor3d::Scene const & scene )
	{
		if ( m_aabbNode )
		{
			auto & obb = m_object->getBoundingBox();

			if ( m_obbNode )
			{
				auto scale = obb.getDimensions() / 2.0f;
				m_obbNode->setScale( scale );
				m_obbNode->setPosition( obb.getCenter() );
			}

			if ( m_obbSelectedSubmeshNode )
			{
				auto & sobb = m_object->getBoundingBox( *m_submesh );
				m_obbSelectedSubmeshNode->setScale( sobb.getDimensions() / 2.0f );
				m_obbSelectedSubmeshNode->setPosition( sobb.getCenter() );
			}

			uint32_t i = 0u;

			for ( auto & submesh : *m_object->getMesh().lock() )
			{
				if ( submesh.get() != m_submesh )
				{
					auto & ssobb = m_object->getBoundingBox( *submesh );
					m_obbSubmeshNodes[i]->setScale( ssobb.getDimensions() / 2.0f );
					m_obbSubmeshNodes[i]->setPosition( ssobb.getCenter() );
					++i;
				}
			}

			auto aabb = obb.getAxisAligned( m_object->getParent()->getDerivedTransformationMatrix() );
			auto aabbMin = aabb.getMin();
			auto aabbMax = aabb.getMax();
			auto aabbSubmesh = m_aabbMesh.lock()->getSubmesh( 0u );
			aabbSubmesh->getPositions()[0u] = castor::Point4f( aabbMin->x, aabbMin->y, aabbMin->z, 1.0f );
			aabbSubmesh->getPositions()[1u] = castor::Point4f( aabbMin->x, aabbMax->y, aabbMin->z, 1.0f );
			aabbSubmesh->getPositions()[2u] = castor::Point4f( aabbMax->x, aabbMax->y, aabbMin->z, 1.0f );
			aabbSubmesh->getPositions()[3u] = castor::Point4f( aabbMax->x, aabbMin->y, aabbMin->z, 1.0f );
			aabbSubmesh->getPositions()[4u] = castor::Point4f( aabbMin->x, aabbMin->y, aabbMax->z, 1.0f );
			aabbSubmesh->getPositions()[5u] = castor::Point4f( aabbMin->x, aabbMax->y, aabbMax->z, 1.0f );
			aabbSubmesh->getPositions()[6u] = castor::Point4f( aabbMax->x, aabbMax->y, aabbMax->z, 1.0f );
			aabbSubmesh->getPositions()[7u] = castor::Point4f( aabbMax->x, aabbMin->y, aabbMax->z, 1.0f );
			aabbSubmesh->needsUpdate();

			castor3d::Engine * engine = m_scene.getEngine();
			engine->postEvent( castor3d::makeGpuFunctorEvent( castor3d::EventType::ePreRender
				, [aabbSubmesh]( castor3d::RenderDevice const & device
					, castor3d::QueueData const & queueData )
				{
					aabbSubmesh->update();
				} ) );
		}
	}
}
