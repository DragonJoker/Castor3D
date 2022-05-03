/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_MeshImporter___
#define ___C3DAssimp_MeshImporter___

#include "MaterialImporter.hpp"
#include "SkeletonImporter.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h> // Output data structure
#pragma warning( pop )

namespace c3d_assimp
{
	struct SubmeshData
	{
		std::vector< castor3d::MaterialRPtr > materials;
	};
	
	struct MeshData
	{
		castor3d::MeshRes mesh;
		castor3d::MeshResPtr lmesh;
		std::set< uint32_t > aiMeshIndices;
		std::map< uint32_t, SubmeshData > submeshes;
	};
	
	using MeshIndices = std::map< uint32_t, castor3d::SubmeshSPtr >;

	class MeshesImporter
	{
	public:
		MeshesImporter( castor3d::MeshImporter & importer
			, MaterialImporter const & materials
			, SkeletonImporter const & skeletons
			, castor3d::MeshPtrStrMap & meshes );

		MeshIndices import( castor::String const & prefix
			, castor::Path const & fileName
			, aiScene const & aiScene
			, castor3d::Mesh & mesh );
		std::map< uint32_t, MeshData * > import( castor::String const & prefix
			, castor::Path const & fileName
			, aiScene const & aiScene
			, castor3d::Scene & scene );

	private:
		void doProcessMeshesAndAnims( aiScene const & aiScene
			, castor3d::Scene & scene
			, castor::ArrayView< aiMesh * > aiMeshes );
		std::map< uint32_t, MeshData * > doRemoveDuplicateMeshes( castor3d::Scene & scene );
		MeshIndices doProcessMeshAndAnims( aiScene const & aiScene
			, castor::ArrayView< aiMesh * > aiMeshes
			, castor3d::Mesh & mesh );
		bool doProcessMesh( castor3d::Scene & scene
			, castor3d::Mesh & mesh
			, castor3d::SkeletonSPtr skeleton
			, aiMesh const & aiMesh
			, uint32_t aiMeshIndex
			, aiScene const & aiScene
			, castor3d::Submesh & submesh );
		void doProcessAnim( aiScene const & aiScene
			, aiMesh const & aiMesh
			, uint32_t aiMeshIndex
			, castor3d::Mesh & mesh
			, castor3d::Submesh & submesh );
		void doFillBonesData( castor3d::Skeleton & skeleton
			, castor::ArrayView< aiBone * > aiBones
			, std::vector< castor3d::VertexBoneData > & arrayVertices );

	private:
		castor3d::MeshImporter & m_importer;
		MaterialImporter const & m_materials;
		SkeletonImporter const & m_skeletons;
		castor3d::MeshPtrStrMap & m_meshes;
		castor::String m_prefix;
		castor::Path m_fileName;
		std::vector< MeshData > m_loadedMeshes;
		castor3d::MeshPtrStrMap m_registeredMeshes;
	};
}

#endif
