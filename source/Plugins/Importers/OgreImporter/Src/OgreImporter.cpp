#include "OgreImporter.hpp"

#include <Cache/GeometryCache.hpp>
#include <Cache/MaterialCache.hpp>
#include <Cache/MeshCache.hpp>
#include <Cache/PluginCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>

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

	bool OgreImporter::DoImportScene( Scene & p_scene )
	{
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_OGRE" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		return l_return;
	}

	bool OgreImporter::DoImportMesh( Mesh & p_mesh )
	{
		bool l_return{ false };
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
		Ogre::Pass::processPendingPassUpdates(); // make sure passes are cleaned up

		return l_return;
	}
}
