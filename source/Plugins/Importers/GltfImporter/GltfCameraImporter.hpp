/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfCameraImporter___
#define ___C3D_GltfCameraImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Scene/CameraImporter.hpp>

namespace c3d_gltf
{
	class GltfCameraImporter
		: public castor3d::CameraImporter
	{
	public:
		explicit GltfCameraImporter( castor3d::Engine & engine );

	private:
		bool doImportCamera( castor3d::Camera & camera )override;
	};
}

#endif
