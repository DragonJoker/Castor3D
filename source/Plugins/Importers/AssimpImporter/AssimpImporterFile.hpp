/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpImporterFile___
#define ___C3D_AssimpImporterFile___

#include "AssimpImporter/AssimpHelpers.hpp"

#include <Castor3D/ImporterFile.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/types.h>
#pragma warning( pop )

namespace c3d_assimp
{
	using SkeletonAnimations = std::map< castor::String, aiAnimation const * >;
	using MeshAnimations = std::map< castor::String, std::pair< aiMesh const *, aiMeshMorphAnim const * > >;
	using NodeAnimations = std::map< castor::String, std::pair< aiAnimation const *, aiNodeAnim const * > >;

	struct SkeletonData
	{
		explicit SkeletonData( aiNode const * prootNode )
			: rootNode{ prootNode }
		{
		}

		aiNode const * rootNode;
		SkeletonAnimations anims;
	};

	struct SubmeshData
	{
		SubmeshData( aiMesh const * pmesh
			, uint32_t pmeshIndex )
			: mesh{ pmesh }
			, meshIndex{ pmeshIndex }
		{
		}

		aiMesh const * mesh;
		uint32_t meshIndex;
		MeshAnimations anims;
	};

	struct MeshData
	{
		explicit MeshData( aiNode const * pskelNode )
			: skelNode{ pskelNode }
		{
		}

		aiNode const * skelNode;
		std::vector< SubmeshData > submeshes;
	};

	struct NodeData
	{
		NodeData( castor::String pparent
			, castor::String pname
			, aiNode const * pnode
			, castor::Matrix4x4f ptransform )
			: parent{ std::move( pparent ) }
			, name{ std::move( pname ) }
			, node{ pnode }
			, transform{ std::move( ptransform ) }
		{
			CU_Require( parent.empty()
				|| parent != name );
		}

		castor::String parent{};
		castor::String name{};
		aiNode const * node{};
		castor::Matrix4x4f transform{};
		std::vector< MeshData const * > meshes{};
		NodeAnimations anims{};
	};

	struct SceneData
	{
		std::map< castor::String, aiMaterial const * > materials;
		std::vector< NodeData > nodes;
		std::map< castor::String, MeshData > meshes;
		std::map< castor::String, SkeletonData > skeletons;
		std::map< castor::String, aiLight const * > lights;
	};

	class AssimpImporterFile
		: public castor3d::ImporterFile
	{
	public:
		AssimpImporterFile( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters );

		static castor3d::ImporterFileUPtr create( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters );

		using castor3d::ImporterFile::getInternalName;

		std::vector< castor::String > listMaterials()override;
		std::vector< std::pair< castor::String, castor::String > > listMeshes()override;
		std::vector< castor::String > listSkeletons()override;
		std::vector< castor::String > listSceneNodes()override;
		std::vector< std::pair< castor::String, castor3d::LightType > > listLights()override;
		std::vector< GeometryData > listGeometries()override;
		std::vector< castor::String > listMeshAnimations( castor3d::Mesh const & mesh )override;
		std::vector< castor::String > listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		std::vector< castor::String > listSceneNodeAnimations( castor3d::SceneNode const & node )override;

		castor3d::MaterialImporterUPtr createMaterialImporter()override;
		castor3d::AnimationImporterUPtr createAnimationImporter()override;
		castor3d::SkeletonImporterUPtr createSkeletonImporter()override;
		castor3d::MeshImporterUPtr createMeshImporter()override;
		castor3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		castor3d::LightImporterUPtr createLightImporter()override;

		castor::String getMaterialName( uint32_t materialIndex )const;
		NodeAnimations const & getNodesAnimations( castor3d::SceneNode const & node )const;
		SkeletonAnimations const & getSkeletonsAnimations( castor3d::Skeleton const & skeleton )const;
		MeshAnimations const & getMeshesAnimations( castor3d::Mesh const & mesh
			, uint32_t submeshIndex )const;

		aiScene const & getAiScene()const
		{
			CU_Require( m_aiScene );
			return *m_aiScene;
		}

		auto const & getMaterials()const
		{
			return m_sceneData.materials;
		}

		auto const & getLights()const
		{
			return m_sceneData.lights;
		}

		auto const & getNodes()const
		{
			return m_sceneData.nodes;
		}

		auto const & getSkeletons()const
		{
			return m_sceneData.skeletons;
		}

		auto const & getMeshes()const
		{
			return m_sceneData.meshes;
		}

		auto const & getListedSkeletons()const
		{
			return m_listedSkeletons;
		}

		auto const & getListedMeshes()const
		{
			return m_listedMeshes;
		}

		auto const & getBonesNodes()const
		{
			return m_bonesNodes;
		}

		castor::String getInternalName( aiString const & name )const
		{
			return getInternalName( makeString( name ) );
		}

		std::vector< castor3d::SubmeshAnimationBuffer > const & addMeshAnimBuffers( aiMesh const * aiMesh
			, std::vector< castor3d::SubmeshAnimationBuffer > buffers )
		{
			return m_meshAnimBuffers.emplace( aiMesh, std::move( buffers ) ).first->second;
		}

		bool hasMeshAnimBuffers( aiMesh const * aiMesh )const
		{
			return m_meshAnimBuffers.end() != m_meshAnimBuffers.find( aiMesh );
		}

		std::vector< castor3d::SubmeshAnimationBuffer > const & getMeshAnimBuffers( aiMesh const * aiMesh )const
		{
			return m_meshAnimBuffers.find( aiMesh )->second;
		}

	public:
		static castor::String const Name;

	private:
		void doPrelistMaterials();
		std::map< aiMesh const *, aiNode const * > doPrelistSkeletons();
		void doPrelistMeshes( std::map< aiMesh const *, aiNode const * > const & meshSkeletons );
		void doPrelistSceneNodes( aiNode const & aiNode
			, std::map< MeshData const *, castor::String > & processedMeshes
			, castor::String parentName = castor::String{}
			, castor::Matrix4x4f transform = castor::Matrix4x4f{ 1.0f } );
		void doPrelistLights();

	private:
		Assimp::Importer m_importer;
		aiScene const * m_aiScene{};
		std::map< castor::String, castor::Matrix4x4f > m_bonesNodes;
		std::map< aiMesh const *, std::vector< castor3d::SubmeshAnimationBuffer > > m_meshAnimBuffers;
		std::set< uint32_t > m_meshes;
		std::vector< castor::String > m_listedMeshes;
		std::vector< castor::String > m_listedSkeletons;

		SceneData m_sceneData;
	};
}

#endif
