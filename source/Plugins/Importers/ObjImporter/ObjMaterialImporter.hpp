/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjMaterialImporter___
#define ___C3D_ObjMaterialImporter___

#include "ObjImporter/ObjImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_obj
{
	class ObjMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		explicit ObjMaterialImporter( c3d::Engine & engine );

		bool importMaterial( c3d::Material & material )override;
	};
}

#endif
