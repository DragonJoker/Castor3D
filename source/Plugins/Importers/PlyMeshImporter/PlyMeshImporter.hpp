/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PlyMeshImporter_H___
#define ___C3D_PlyMeshImporter_H___

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

namespace c3d_ply
{
	class PlyMeshFile;

	class PlyMeshImporter
		: public c3d::MeshImporter
	{
	public:
		explicit PlyMeshImporter( c3d::Engine & engine );

	private:
		bool doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )override;
	};
}

#endif
