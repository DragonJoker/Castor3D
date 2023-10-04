#include "AssimpImporter/AssimpCameraImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Camera.hpp>

namespace c3d_assimp
{
	AssimpCameraImporter::AssimpCameraImporter( castor3d::Engine & engine )
		: castor3d::CameraImporter{ engine }
	{
	}

	bool AssimpCameraImporter::doImportCamera( castor3d::Camera & camera )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = camera.getName();
		auto it = file.getCameras().find( name );

		if ( it == file.getCameras().end() )
		{
			return false;
		}

		auto & aiCamera = *it->second;
		auto & scene = *camera.getScene();
		auto & viewport = camera.getViewport();
		castor3d::log::info << cuT( "  Camera found: [" ) << name << cuT( "]" ) << std::endl;
		castor3d::SceneNodeRPtr node{};

		if ( scene.hasSceneNode( name ) )
		{
			node = scene.findSceneNode( name );
		}
		else
		{
			auto ownNode = scene.createSceneNode( name, scene );
			ownNode->attachTo( *scene.getObjectRootNode() );
			node = scene.addSceneNode( name, ownNode );
		}

		if ( aiCamera.mOrthographicWidth == 0.0f )
		{
			viewport.setPerspective( castor::Angle::fromRadians( aiCamera.mHorizontalFOV * aiCamera.mAspect )
				, aiCamera.mAspect
				, aiCamera.mClipPlaneNear
				, aiCamera.mClipPlaneFar );
		}
		else
		{
			viewport.setOrtho( -aiCamera.mOrthographicWidth
				, aiCamera.mOrthographicWidth
				, -aiCamera.mOrthographicWidth * aiCamera.mAspect
				, aiCamera.mOrthographicWidth * aiCamera.mAspect
				, aiCamera.mClipPlaneNear
				, aiCamera.mClipPlaneFar );
		}

		node->attachObject( camera );
		return true;
	}
}
