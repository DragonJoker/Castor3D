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
		: public c3d::MeshImporter
	{
	public:
		explicit AssimpMeshImporter( c3d::Engine & engine );

	private:
		bool doImportMesh( c3d::Mesh & mesh, uint32_t submesh )override;
		void doImportSingleMesh( c3d::Mesh & mesh, uint32_t submesh );
		bool doImportSceneMesh( c3d::Mesh & mesh, uint32_t submesh );
		void doProcessMesh( aiScene const & aiScene
			, aiMesh const & aiMesh
			, uint32_t aiMeshIndex
			, c3d::Mesh & mesh
			, c3d::Submesh & submesh );
		void doTransformMesh( aiNode const & aiNode
			, c3d::Mesh & mesh
			, aiMatrix4x4 transformAcc = aiMatrix4x4{} );
	};
}

#endif
