/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMaterialImporter___
#define ___C3D_GltfMaterialImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_gltf
{
	class GltfMaterialImporter
		: public castor3d::MaterialImporter
	{
	public:
		C3D_Gltf_API explicit GltfMaterialImporter( castor3d::Engine & engine );
		C3D_Gltf_API explicit GltfMaterialImporter( castor3d::Engine & engine
			, GltfImporterFile * file );

	private:
		bool doImportMaterial( castor3d::Material & material )override;
	};
}

#endif
