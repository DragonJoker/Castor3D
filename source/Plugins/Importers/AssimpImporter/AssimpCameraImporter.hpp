/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpCameraImporter___
#define ___C3D_AssimpCameraImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Scene/CameraImporter.hpp>

namespace c3d_assimp
{
	class AssimpCameraImporter
		: public c3d::CameraImporter
	{
	public:
		explicit AssimpCameraImporter( c3d::Engine & engine );

	private:
		bool doImportCamera( c3d::Camera & camera )override;
	};
}

#endif
