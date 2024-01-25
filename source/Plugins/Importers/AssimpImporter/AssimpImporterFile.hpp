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
	using SkeletonAnimations = castor::StringMap< aiAnimation const * >;
	using MeshAnimations = castor::StringMap< castor::Pair< aiMesh const *, aiMeshMorphAnim const * > >;
	using NodeAnimations = castor::StringMap< castor::Pair< aiAnimation const *, aiNodeAnim const * > >;
	using aiNodeArray = castor::Vector< aiNode const * >;

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
		castor::Vector< AssimpSubmeshData > submeshes;
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
			: NodeData{ castor::move( pparent )
				, castor::move( pname )
				, pisCamera }
			, node{ pnode }
			, translate{ castor::move( ptranslate ) }
			, rotate{ castor::move( protate ) }
			, scale{ castor::move( pscale ) }
		{
		}

		aiNode const * node{};
		castor::Point3f translate{};
		castor::Quaternion rotate{};
		castor::Point3f scale{};
		castor::Vector< AssimpMeshData const * > meshes{};
		NodeAnimations anims{};
	};

	struct AssimpSceneData
	{
		castor::StringMap< aiMaterial const * > materials;
		castor::Vector< AssimpNodeData > nodes;
		castor::StringMap< AssimpMeshData > meshes;
		castor::StringMap< AssimpSkeletonData > skeletons;
		castor::StringMap< aiLight const * > lights;
		castor::StringMap< aiCamera const * > cameras;
	};

	class AssimpImporterFile
		: public castor3d::ImporterFile
	{
	public:
		AssimpImporterFile( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters
			, castor3d::ProgressBar * progress );

		static castor3d::ImporterFileUPtr create( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters
			, castor3d::ProgressBar * progress );

		using castor3d::ImporterFile::getInternalName;

		castor::StringArray listMaterials()override;
		castor::Vector< MeshData > listMeshes()override;
		castor::StringArray listSkeletons()override;
		castor::Vector< NodeData > listSceneNodes()override;
		castor::Vector< LightData > listLights()override;
		castor::Vector< GeometryData > listGeometries()override;
		castor::Vector< CameraData > listCameras()override;
		castor::StringArray listMeshAnimations( castor3d::Mesh const & mesh )override;
		castor::StringArray listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		castor::StringArray listSceneNodeAnimations( castor3d::SceneNode const & node )override;
		castor::StringArray listAllMeshAnimations()override;
		castor::StringArray listAllSkeletonAnimations()override;
		castor::StringArray listAllSceneNodeAnimations()override;

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
		static castor::MbString const Name;

	private:
		void doPrelistMaterials();
		castor::Map< aiMesh const *, aiNode const * > doPrelistSkeletons();
		void doPrelistMeshes( castor::Map< aiMesh const *, aiNode const * > const & meshSkeletons );
		void doPrelistSceneNodes( aiNode const & node
			, castor::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes
			, castor::Map< aiNode const *, castor::Matrix4x4f > & cumulativeTransforms
			, castor::String parentName = castor::String{}
			, castor::Matrix4x4f transform = castor::Matrix4x4f{ 1.0f } );
		void doPrelistLights();
		void doPrelistCameras();

	private:
		Assimp::Importer m_importer;
		aiScene const * m_aiScene{};
		castor::StringMap< castor::Matrix4x4f > m_bonesNodes;
		castor::Set< uint32_t > m_meshes;
		castor::StringArray m_listedMeshes;
		castor::StringArray m_listedSkeletons;

		AssimpSceneData m_sceneData;
	};
}

#endif
