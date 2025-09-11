/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMaterialImporter___
#define ___C3D_GltfMaterialImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Material/MaterialImporter.hpp>

namespace c3d_gltf
{
	c3d::WrapMode convert( fastgltf::Wrap const & v );
	c3d::FilterMode convert( fastgltf::Filter const & v );
	c3d::MipmapMode getMipFilter( fastgltf::Filter const & v );

	class GltfMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		C3D_Gltf_API explicit GltfMaterialImporter( c3d::Engine & engine );
		C3D_Gltf_API explicit GltfMaterialImporter( c3d::Engine & engine
			, GltfImporterFile * file );

		C3D_Gltf_API bool importMaterial( c3d::Material & material )override;

	private:
		void doImportSpecularData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportIridescenceData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportVolumeData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportTransmissionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportClearcoatData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportSheenData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportEmissiveData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportAnisotropyData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportDiffuseTransmissionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass );
		void doImportAlphaModeData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportIorData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportDispersionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
	};
}

#endif
