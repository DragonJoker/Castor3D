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
		: public c3d::SceneNodeImporter
	{
	public:
		explicit AssimpSceneNodeImporter( c3d::Engine & engine );

	private:
		bool doImportSceneNode( c3d::SceneNode & node )override;
	};
}

#endif
