/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMaterialImporter___
#define ___C3D_GltfMaterialImporter___

#include <Castor3D/Material/MaterialImporter.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#ifdef None
#	undef None
#endif
#pragma warning( disable: 4715 )
#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_gltf
{
	class GltfMaterialsFile;

	c3d::WrapMode convert( fastgltf::Wrap const & v );
	c3d::FilterMode convert( fastgltf::Filter const & v );
	c3d::MipmapMode getMipFilter( fastgltf::Filter const & v );

	class GltfMaterialImporter
		: public c3d::MaterialImporter
	{
	public:
		explicit GltfMaterialImporter( c3d::Engine & engine
			, GltfMaterialsFile const & materialsFile );

		bool importMaterial( c3d::Material & material )override;

	private:
		void doImportSpecularData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportIridescenceData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportVolumeData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportTransmissionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportClearcoatData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportSheenData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportEmissiveData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportAnisotropyData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportDiffuseTransmissionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportAlphaModeData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportIorData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;
		void doImportDispersionData( fastgltf::Material const & impMaterial
			, c3d::Pass & pass )const;

	private:
		GltfMaterialsFile const & m_materialsFile;
	};
}

#endif
