/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpSceneNodeImporter___
#define ___C3D_AssimpSceneNodeImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Scene/SceneNodeImporter.hpp>

namespace c3d_assimp
{
	class AssimpSceneNodeImporter
		: public castor3d::SceneNodeImporter
	{
	public:
		explicit AssimpSceneNodeImporter( castor3d::Engine & engine );

	private:
		bool doImportSceneNode( castor3d::SceneNode & node )override;
	};
}

#endif
