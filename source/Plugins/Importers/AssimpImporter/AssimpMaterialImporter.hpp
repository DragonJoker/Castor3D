/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpMaterialImporter___
#define ___C3D_AssimpMaterialImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_assimp
{
	class AssimpMaterialImporter
		: public castor3d::MaterialImporter
	{
	public:
		explicit AssimpMaterialImporter( castor3d::Engine & engine );

	private:
		bool doImportMaterial( castor3d::Material & material )override;
	};
}

#endif
