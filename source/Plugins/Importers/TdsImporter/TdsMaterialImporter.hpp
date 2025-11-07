/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TdsMaterialImporter___
#define ___C3D_TdsMaterialImporter___

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_3ds
{
	class TdsMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		explicit TdsMaterialImporter( c3d::Engine & engine );

		bool importMaterial( c3d::Material & material )override;
	};
}

#endif
