/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfLightImporter___
#define ___C3D_GltfLightImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Scene/Light/LightImporter.hpp>

namespace c3d_gltf
{
	class GltfLightImporter
		: public castor3d::LightImporter
	{
	public:
		explicit GltfLightImporter( castor3d::Engine & engine );

	private:
		bool doImportLight( castor3d::Light & light )override;
		bool doImportLightGroup( castor3d::LightGroup & light )override;
		bool doImportLightCategory( fastgltf::Light const & impLight
			, castor3d::LightCategory & category );
		castor3d::SceneNode * doLoadNode( castor3d::Scene & scene
			, castor::String const & nodeName
			, bool invertY );
	};
}

#endif
