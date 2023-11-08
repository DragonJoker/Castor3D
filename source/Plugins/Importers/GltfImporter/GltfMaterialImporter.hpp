/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMaterialImporter___
#define ___C3D_GltfMaterialImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_gltf
{
	VkSamplerAddressMode convert( fastgltf::Wrap const & v );
	VkFilter convert( fastgltf::Filter const & v );
	VkSamplerMipmapMode getMipFilter( fastgltf::Filter const & v );

	class GltfMaterialImporter
		: public castor3d::MaterialImporter
	{
	public:
		C3D_Gltf_API explicit GltfMaterialImporter( castor3d::Engine & engine );
		C3D_Gltf_API explicit GltfMaterialImporter( castor3d::Engine & engine
			, GltfImporterFile * file );

	private:
		bool doImportMaterial( castor3d::Material & material )override;

		void doImportSpecularData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportIridescenceData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportVolumeData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportTransmissionData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportClearcoatData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportSheenData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportEmissiveData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportAlphaModeData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
		void doImportIorData( fastgltf::Material const & impMaterial
			, castor3d::Pass & pass );
	};
}

#endif
