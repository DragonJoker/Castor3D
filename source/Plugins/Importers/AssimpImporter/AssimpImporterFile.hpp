/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpImporterFile___
#define ___C3D_AssimpImporterFile___

#include "AssimpImporter/AssimpHelpers.hpp"
#if C3D_HasFbxMaterialImporter
#	include <FbxMaterialImporter/FbxMaterialsFile.hpp>
#endif
#if C3D_HasPlyMeshImporter
#	include <PlyMeshImporter/PlyMeshFile.hpp>
#endif

#include <Castor3D/ImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Assimp_API
#else
#	ifdef AssimpImporter_EXPORTS
#		define C3D_Assimp_API __declspec(dllexport)
#	else
#		define C3D_Assimp_API __declspec(dllimport)
#	endif
#endif

namespace c3d_assimp
{
	using SkeletonAnimations = c3d::StringMap< aiAnimation const * >;
	using MeshAnimations = c3d::StringMap< c3d::Pair< aiMesh const *, aiMeshMorphAnim const * > >;
	using NodeAnimations = c3d::StringMap< c3d::Pair< aiAnimation const *, aiNodeAnim const * > >;
	using aiNodeArray = c3d::Vector< aiNode const * >;

	struct NameContainer
	{
		c3d::HashMap< c3d::u32, c3d::String > namesByIndex;
		c3d::HashMap< c3d::String, c3d::String > namesByRawName;
		c3d::HashSet< c3d::String > names;
	};

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
		c3d::Vector< AssimpSubmeshData > submeshes;
	};

	struct AssimpNodeData
		: c3d::ImporterFile::NodeData
	{
		AssimpNodeData( c3d::String pparent
			, c3d::String pname
			, bool pisCamera
			, aiNode const * pnode
			, c3d::Point3f ptranslate
			, c3d::Quaternion protate
			, c3d::Point3f pscale )
			: NodeData{ c3d::move( pparent )
				, c3d::move( pname )
				, pisCamera }
			, node{ pnode }
			, translate{ c3d::move( ptranslate ) }
			, rotate{ c3d::move( protate ) }
			, scale{ c3d::move( pscale ) }
		{
		}

		aiNode const * node{};
		c3d::Point3f translate{};
		c3d::Quaternion rotate{};
		c3d::Point3f scale{};
		c3d::Vector< AssimpMeshData const * > meshes{};
		NodeAnimations anims{};
	};

	struct AssimpSceneData
	{
		c3d::StringMap< aiMaterial const * > materials;
		c3d::Vector< AssimpNodeData > nodes;
		c3d::StringMap< AssimpMeshData > meshes;
		c3d::StringMap< AssimpSkeletonData > skeletons;
		c3d::StringMap< aiLight const * > lights;
		c3d::StringMap< aiCamera const * > cameras;
	};

	class AssimpImporterFile
		: public c3d::ImporterFile
	{
	public:
		C3D_Assimp_API AssimpImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		using c3d::ImporterFile::getInternalName;

		C3D_Assimp_API c3d::String getMaterialName( c3d::u32 index )const;
		C3D_Assimp_API c3d::String getMeshName( c3d::u32 index )const;
		C3D_Assimp_API c3d::String getSkinName( c3d::u32 index )const;
		C3D_Assimp_API c3d::String getLightName( c3d::u32 index )const;
		C3D_Assimp_API c3d::String getCameraName( c3d::u32 index )const;
		C3D_Assimp_API c3d::String getAnimationName( c3d::u32 index )const;

		C3D_Assimp_API c3d::StringArray listMaterials()override;
		C3D_Assimp_API c3d::Vector< MeshData > listMeshes()override;
		C3D_Assimp_API c3d::StringArray listSkeletons()override;
		C3D_Assimp_API c3d::Vector< NodeData > listSceneNodes()override;
		C3D_Assimp_API c3d::Vector< LightData > listLights()override;
		C3D_Assimp_API c3d::Vector< LightGroupData > listLightGroups()override;
		C3D_Assimp_API c3d::Vector< GeometryData > listGeometries()override;
		C3D_Assimp_API c3d::Vector< CameraData > listCameras()override;
		C3D_Assimp_API c3d::StringArray listMeshAnimations( c3d::Mesh const & mesh )override;
		C3D_Assimp_API c3d::StringArray listSkeletonAnimations( c3d::Skeleton const & skeleton )override;
		C3D_Assimp_API c3d::StringArray listSceneNodeAnimations( c3d::SceneNode const & node )override;
		C3D_Assimp_API c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		C3D_Assimp_API uint32_t countAllMeshAnimations()const override;
		C3D_Assimp_API uint32_t countAllSkeletonAnimations()const override;
		C3D_Assimp_API uint32_t countAllSceneNodeAnimations()const override;
		C3D_Assimp_API uint32_t countAllTextureAnimations()const override;

		C3D_Assimp_API c3d::MaterialImporterUPtr createMaterialImporter()override;
		C3D_Assimp_API c3d::AnimationImporterUPtr createAnimationImporter()override;
		C3D_Assimp_API c3d::SkeletonImporterUPtr createSkeletonImporter()override;
		C3D_Assimp_API c3d::MeshImporterUPtr createMeshImporter()override;
		C3D_Assimp_API c3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		C3D_Assimp_API c3d::LightImporterUPtr createLightImporter()override;
		C3D_Assimp_API c3d::CameraImporterUPtr createCameraImporter()override;

		NodeAnimations const & getNodesAnimations( c3d::SceneNode const & node )const;
		SkeletonAnimations const & getSkeletonsAnimations( c3d::Skeleton const & skeleton )const;
		MeshAnimations const & getMeshesAnimations( c3d::Mesh const & mesh
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

		c3d::String getInternalName( aiString const & name )const
		{
			return getInternalName( makeString( name ) );
		}

		auto const & getMaterialsNames()const noexcept
		{
			return m_materialNames.namesByRawName;
		}

	public:
		static c3d::MbString const Name;

	private:
		void doPrelistMaterials();
		c3d::Map< aiMesh const *, aiNode const * > doPrelistSkeletons();
		void doPrelistMeshes( c3d::Map< aiMesh const *, aiNode const * > const & meshSkeletons );
		void doPrelistSceneNodes( aiNode const & node
			, c3d::Map< AssimpMeshData const *, aiNodeArray > & processedMeshes
			, c3d::Map< aiNode const *, c3d::Matrix4x4f > & cumulativeTransforms
			, c3d::String parentName = c3d::String{}
			, c3d::Matrix4x4f transform = c3d::Matrix4x4f{ 1.0f } );
		void doPrelistLights();
		void doPrelistCameras();

	private:
		Assimp::Importer m_importer;
		aiScene const * m_aiScene{};
		c3d::StringMap< c3d::Matrix4x4f > m_bonesNodes;
		c3d::Set< uint32_t > m_meshes;
		c3d::StringArray m_listedMeshes;
		c3d::StringArray m_listedSkeletons;

		AssimpSceneData m_sceneData;
		mutable NameContainer m_materialNames;
		mutable NameContainer m_meshNames;
		mutable NameContainer m_nodeNames;
		mutable NameContainer m_skinNames;
		mutable NameContainer m_lightNames;
		mutable NameContainer m_cameraNames;
		mutable NameContainer m_samplerNames;
		mutable NameContainer m_animationNames;

#if C3D_HasFbxMaterialImporter
		c3d::RawUniquePtr< c3d_fbx::FbxMaterialsFile > m_fbxMaterials;
#endif
#if C3D_HasPlyMeshImporter
		c3d::RawUniquePtr< c3d_ply::PlyMeshFile > m_plyMesh;
#endif
	};
}

#endif
