#include "GltfImporter/GltfCameraImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Camera.hpp>

namespace c3d_gltf
{
	GltfCameraImporter::GltfCameraImporter( castor3d::Engine & engine )
		: castor3d::CameraImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfCameraImporter::doImportCamera( castor3d::Camera & camera )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = camera.getName();
		uint32_t cameraIndex{};
		auto it = std::find_if( impAsset.cameras.begin()
			, impAsset.cameras.end()
			, [&name, &file, &cameraIndex]( fastgltf::Camera const & lookup )
			{
				return name == file.getCameraName( cameraIndex++ );
			} );

		if ( it == impAsset.cameras.end() )
		{
			return false;
		}

		fastgltf::Camera const & impCamera = *it;
		--cameraIndex;
		auto nodeIt = std::find_if( impAsset.nodes.begin()
			, impAsset.nodes.end()
			, [&cameraIndex]( fastgltf::Node const & lookup )
			{
				return lookup.cameraIndex
					&& *lookup.cameraIndex == cameraIndex;
			} );
		auto nodeIndex = uint32_t( std::distance( impAsset.nodes.begin(), nodeIt ) );
		auto nodeName = file.getNodeName( nodeIndex, 0u );
		auto & scene = *camera.getScene();
		castor3d::SceneNodeRPtr node{};

		if ( scene.hasSceneNode( nodeName ) )
		{
			node = scene.findSceneNode( nodeName );
		}
		else
		{
			auto ownNode = scene.createSceneNode( nodeName, scene );
			ownNode->attachTo( *scene.getCameraRootNode() );
			node = scene.addSceneNode( nodeName, ownNode );
		}

		auto & viewport = camera.getViewport();

		if ( impCamera.camera.index() == 0u )
		{
			auto & perspective = std::get< 0u >( impCamera.camera );

			if ( !perspective.zfar )
			{
				viewport.setInfinitePerspective( castor::Angle::fromRadians( perspective.yfov )
					, perspective.aspectRatio ? *perspective.aspectRatio : 1.0f
					, perspective.znear );
			}
			else
			{
				viewport.setPerspective( castor::Angle::fromRadians( perspective.yfov )
					, perspective.aspectRatio ? *perspective.aspectRatio : 1.0f
					, perspective.znear
					, *perspective.zfar );
			}
		}
		else
		{
			auto & ortho = std::get< 1u >( impCamera.camera );
			viewport.setOrtho( -ortho.xmag, ortho.xmag
				, -ortho.ymag, ortho.ymag
				, -ortho.znear, ortho.zfar );
		}

		node->attachObject( camera );
		return true;
	}
}
