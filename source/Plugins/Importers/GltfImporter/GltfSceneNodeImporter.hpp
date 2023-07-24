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
		: public castor3d::SceneNodeImporter
	{
	public:
		explicit GltfSceneNodeImporter( castor3d::Engine & engine
			, uint32_t instanceIndex = 0u );

	private:
		bool doImportSceneNode( castor3d::SceneNode & node )override;

	private:
		uint32_t m_instanceIndex;
	};
}

#endif
