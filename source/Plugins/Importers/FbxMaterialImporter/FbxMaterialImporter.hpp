/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FbxMaterialImporter___
#define ___C3D_FbxMaterialImporter___

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_fbx
{
	class FbxMaterialsFile;

	class FbxMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		explicit FbxMaterialImporter( c3d::Engine & engine
			, FbxMaterialsFile const & materialsFile );

		bool importMaterial( c3d::Material & material )override;

	private:
		FbxMaterialsFile const & m_materialsFile;
	};
}

#endif
