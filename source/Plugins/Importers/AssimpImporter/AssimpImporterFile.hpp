/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpImporterFile___
#define ___C3D_AssimpImporterFile___

#include "AssimpImporter/AssimpHelpers.hpp"

#include <Castor3D/ImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_assimp
{
	using SkeletonAnimations = std::map< castor::String, aiAnimation const * >;
	using MeshAnimations = std::map< castor::String, std::pair< aiMesh const *, aiMeshMorphAnim const * > >;
	using NodeAnimations = std::map< castor::String, std::pair< aiAnimation const *, aiNodeAnim const * > >;
	using aiNodeArray = std::vector< aiNode const * >;

	struct AssimpSkeletonData
	{
		explicit AssimpSkeletonData( aiNode const * prootNode )
			: rootNode{ prootNode }
		{
		}

		aiNode const * rootNode;
		SkeletonAnimations anims;
	};

	struct AssimpSubmeshData
	{
		AssimpSubmeshData( aiMesh const * pmesh
			, uint32_t pmeshIndex )
			: mesh{ pmesh }
			, meshIndex{ pmeshIndex }
		{
		}

		aiMesh const * mesh;
		uint32_t meshIndex;
		MeshAnimations anims;
	};

	struct AssimpMeshData
	{
		explicit AssimpMeshData( aiNode const * pskelNode )
			: skelNode{ pskelNode }
		{
		}

		aiNode const * skelNode;
		std::vector< AssimpSubmeshData > submeshes;
	};

	struct AssimpNodeData
		: castor3d::ImporterFile::NodeData
	{
		AssimpNodeData( castor::String pparent
			, castor::String pname
			, bool pisCamera
			, aiNode const * pnode
			, castor::Point3f ptranslate
			, castor::Quaternion protate
			, castor::Point3f pscale )
			: NodeData{ std::move( pparent )
				, std::move( pname )
				, pisCamera }
			, node{ pnode }
			, translate{ std::move( ptranslate ) }
			, rotate{ std::move( protate ) }
			, scale{ std::move( pscale ) }
		{
		}

		aiNode const * node{};
		castor::Point3f translate{};
		castor::Quaternion rotate{};
		castor::Point3f scale{};
		std::vector< AssimpMeshData const * > meshes{};
		NodeAnimations anims{};
	};

	struct AssimpSceneData
	{
		std::map< castor::String, aiMaterial const * > materials;
		std::vector< AssimpNodeData > nodes;
		std::map< castor::String, AssimpMeshData > meshes;
		std::map< castor::String, AssimpSkeletonData > skeletons;
		std::map< castor::String, aiLight const * > lights;
		std::map< castor::String, aiCamera const * > cameras;
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
		std::vector< MeshData > listMeshes()override;
		std::vector< castor::String > listSkeletons()override;
		std::vector< NodeData > listSceneNodes()override;
		std::vector< LightData > listLights()override;
		std::vector< GeometryData > listGeometries()override;
		std::vector< CameraData > listCameras()override;
		std::vector< castor::String > listMeshAnimations( castor3d::Mesh const & mesh )override;
		std::vector< castor::String > listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		std::vector< castor::String > listSceneNodeAnimations( castor3d::SceneNode const & node )override;

		castor3d::MaterialImporterUPtr createMaterialImporter()override;
		castor3d::AnimationImporterUPtr createAnimationImporter()override;
		castor3d::SkeletonImporterUPtr createSkeletonImporter()override;
		castor3d::MeshImporterUPtr createMeshImporter()override;
		castor3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		castor3d::LightImporterUPtr createLightImporter()override;
		castor3d::CameraImporterUPtr createCameraImporter()override;

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

		auto const & getCameras()const
		{
			return m_sceneData.cameras;
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

	public:
		static castor::String const Name;

	private:
		void doPrelistMaterials();
		std::map< aiMesh const *, aiNode const * > doPrelistSkeletons();
		void doPrelistMeshes( std::map< aiMesh const *, aiNode const * > const & meshSkeletons );
		void doPrelistSceneNodes( aiNode const & node
			, std::map< AssimpMeshData const *, aiNodeArray > & processedMeshes
			, std::map< aiNode const *, castor::Matrix4x4f > & cumulativeTransforms
			, castor::String parentName = castor::String{}
			, castor::Matrix4x4f transform = castor::Matrix4x4f{ 1.0f } );
		void doPrelistLights();
		void doPrelistCameras();

	private:
		Assimp::Importer m_importer;
		aiScene const * m_aiScene{};
		std::map< castor::String, castor::Matrix4x4f > m_bonesNodes;
		std::set< uint32_t > m_meshes;
		std::vector< castor::String > m_listedMeshes;
		std::vector< castor::String > m_listedSkeletons;

		AssimpSceneData m_sceneData;
	};
}

#endif
