/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FbxMaterialImporter___
#define ___C3D_FbxMaterialImporter___

#include "FbxImporter/FbxImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_fbx
{
	class FbxMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		explicit FbxMaterialImporter( c3d::Engine & engine );

		bool importMaterial( c3d::Material & material )override;
	};
}

#endif
