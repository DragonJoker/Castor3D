/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfSceneNodeImporter___
#define ___C3D_GltfSceneNodeImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Scene/SceneNodeImporter.hpp>

namespace c3d_gltf
{
	class GltfSceneNodeImporter
		: public c3d::SceneNodeImporter
	{
	public:
		explicit GltfSceneNodeImporter( c3d::Engine & engine );

	private:
		bool doImportSceneNode( c3d::SceneNode & node )override;
	};
}

#endif
