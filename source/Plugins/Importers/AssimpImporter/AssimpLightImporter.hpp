/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpLightImporter___
#define ___C3D_AssimpLightImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Scene/Light/LightImporter.hpp>

namespace c3d_assimp
{
	class AssimpLightImporter
		: public castor3d::LightImporter
	{
	public:
		explicit AssimpLightImporter( castor3d::Engine & engine );

	private:
		bool doImportLight( castor3d::Light & light )override;
	};
}

#endif
