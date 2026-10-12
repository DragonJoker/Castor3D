/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HairMeshImporter_H___
#define ___C3D_HairMeshImporter_H___

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

namespace hair
{
	class MeshImporterFile;

	class MeshImporter
		: public c3d::MeshImporter
	{
	public:
		explicit MeshImporter( c3d::Engine & engine
			, c3d::MeshImportConfig const & config );

	private:
		bool doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )override;
	};
}

#endif
