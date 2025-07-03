/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpMeshImporter___
#define ___C3D_AssimpMeshImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Model/Mesh/MeshImporter.hpp>

namespace c3d_assimp
{
	class AssimpMeshImporter
		: public castor3d::MeshImporter
	{
	public:
		explicit AssimpMeshImporter( castor3d::Engine & engine );

	private:
		bool doImportMesh( castor3d::Mesh & mesh, uint32_t submesh )override;
		void doImportSingleMesh( castor3d::Mesh & mesh, uint32_t submesh );
		bool doImportSceneMesh( castor3d::Mesh & mesh, uint32_t submesh );
		void doProcessMesh( aiScene const & aiScene
			, aiMesh const & aiMesh
			, uint32_t aiMeshIndex
			, castor3d::Mesh & mesh
			, castor3d::Submesh & submesh );
		void doTransformMesh( aiNode const & aiNode
			, castor3d::Mesh & mesh
			, aiMatrix4x4 transformAcc = aiMatrix4x4{} );
	};
}

#endif
