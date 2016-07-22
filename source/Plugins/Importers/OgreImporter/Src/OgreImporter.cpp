#include "OgreImporter.hpp"

#include <GeometryCache.hpp>
#include <MaterialCache.hpp>
#include <MeshCache.hpp>
#include <PluginCache.hpp>
#include <SceneCache.hpp>
#include <SceneNodeCache.hpp>

#include <Animation/Mesh/MeshAnimation.hpp>
#include <Animation/Mesh/MeshAnimationSubmesh.hpp>
#include <Animation/Skeleton/SkeletonAnimation.hpp>
#include <Animation/Skeleton/SkeletonAnimationBone.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Plugin/ImporterPlugin.hpp>

#include <Log/Logger.hpp>

#include <Ogre.h>
#include <OgreMeshSerializer.h>
#include <OgreSkeletonSerializer.h>
#include <OgreDefaultHardwareBufferManager.h>

using namespace Castor3D;
using namespace Castor;

namespace C3dOgre
{
	OgreImporter::OgreImporter( Engine & p_engine )
		: Importer{ p_engine }
	{
	}

	OgreImporter::~OgreImporter()
	{
	}

	ImporterUPtr OgreImporter::Create( Engine & p_engine )
	{
		return std::make_unique< OgreImporter >( p_engine );
	}

	SceneSPtr OgreImporter::DoImportScene()
	{
		SceneSPtr l_scene = GetEngine()->GetSceneCache().Add( cuT( "Scene_OGRE" ) );
		DoImportMesh( *l_scene );

		if ( m_mesh )
		{
			SceneNodeSPtr l_node = l_scene->GetSceneNodeCache().Add( m_mesh->GetName(), l_scene->GetObjectRootNode() );

			for ( auto && l_submesh : *m_mesh )
			{
				m_mesh->GetScene()->GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
			}

			GeometrySPtr l_geometry = l_scene->GetGeometryCache().Add( m_mesh->GetName(), l_node, m_mesh );
			m_mesh.reset();
		}

		return l_scene;
	}

	MeshSPtr OgreImporter::DoImportMesh( Scene & p_scene )
	{
		m_mapBoneByID.clear();
		m_arrayBones.clear();

		auto logMgr = std::make_unique< Ogre::LogManager >();
		logMgr->setDefaultLog( logMgr->createLog( "OgreImporter.log", false, false ) );

		auto rgm = std::make_unique< Ogre::ResourceGroupManager >();
		auto mth = std::make_unique< Ogre::Math >();
		auto lodMgr = std::make_unique< Ogre::LodStrategyManager >();
		auto meshMgr = std::make_unique< Ogre::MeshManager >();
		auto matMgr = std::make_unique< Ogre::MaterialManager >();
		matMgr->initialise();
		auto skelMgr = std::make_unique< Ogre::SkeletonManager >();
		auto meshSerializer = std::make_unique< Ogre::MeshSerializer >();
		auto skeletonSerializer = std::make_unique< Ogre::SkeletonSerializer >();
		auto bufferManager = std::make_unique< Ogre::DefaultHardwareBufferManager >(); // needed because we don't have a rendersystem

		std::ifstream ifs{ m_fileName, std::ios_base::in | std::ios_base::binary };

		if ( ifs.bad() )
		{
			CASTOR_EXCEPTION( "Unable to load file " + m_fileName );
		}

		Ogre::MeshPtr mesh = meshMgr->create( "import", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
		Ogre::DataStreamPtr stream{ new Ogre::FileStreamDataStream( m_fileName, &ifs, false ) };
		meshSerializer->importMesh( stream, mesh.getPointer() );


		meshMgr->remove( "import" );

		MeshSPtr l_return( m_mesh );
		m_mesh.reset();

		Ogre::Pass::processPendingPassUpdates(); // make sure passes are cleaned up

		return l_return;
	}
}
