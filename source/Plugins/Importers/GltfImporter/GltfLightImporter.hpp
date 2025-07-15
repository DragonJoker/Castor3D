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
		: public c3d::LightImporter
	{
	public:
		explicit GltfLightImporter( c3d::Engine & engine );

	private:
		bool doImportLight( c3d::Light & light )override;
		bool doImportLightGroup( c3d::LightGroup & light )override;
		void doImportLightCategory( fastgltf::Light const & impLight
			, c3d::LightCategory & category );
		c3d::SceneNode * doLoadNode( c3d::Scene & scene
			, c3d::String const & nodeName
			, bool invertY );
	};
}

#endif
