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
		: public c3d::LightImporter
	{
	public:
		explicit AssimpLightImporter( c3d::Engine & engine );

	private:
		bool doImportLight( c3d::Light & light )override;

		bool doImportLightGroup( c3d::LightGroup & )override
		{
			return false;
		}
	};
}

#endif
