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
	using SkeletonAnimations = c3d::StringMap< aiAnimation const * >;
	using MeshAnimations = c3d::StringMap< c3d::Pair< aiMesh const *, aiMeshMorphAnim const * > >;
	using NodeAnimations = c3d::StringMap< c3d::Pair< aiAnimation const *, aiNodeAnim const * > >;
	using aiNodeArray = c3d::Vector< aiNode const * >;

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
		AssimpImporterFile( c3d::Engine & engine
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

		c3d::StringArray listMaterials()override;
		c3d::Vector< MeshData > listMeshes()override;
		c3d::StringArray listSkeletons()override;
		c3d::Vector< NodeData > listSceneNodes()override;
		c3d::Vector< LightData > listLights()override;
		c3d::Vector< LightGroupData > listLightGroups()override;
		c3d::Vector< GeometryData > listGeometries()override;
		c3d::Vector< CameraData > listCameras()override;
		c3d::StringArray listMeshAnimations( c3d::Mesh const & mesh )override;
		c3d::StringArray listSkeletonAnimations( c3d::Skeleton const & skeleton )override;
		c3d::StringArray listSceneNodeAnimations( c3d::SceneNode const & node )override;
		c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		uint32_t countAllMeshAnimations()const override;
		uint32_t countAllSkeletonAnimations()const override;
		uint32_t countAllSceneNodeAnimations()const override;
		uint32_t countAllTextureAnimations()const override;

		c3d::MaterialImporterUPtr createMaterialImporter()override;
		c3d::AnimationImporterUPtr createAnimationImporter()override;
		c3d::SkeletonImporterUPtr createSkeletonImporter()override;
		c3d::MeshImporterUPtr createMeshImporter()override;
		c3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		c3d::LightImporterUPtr createLightImporter()override;
		c3d::CameraImporterUPtr createCameraImporter()override;

		c3d::String getMaterialName( uint32_t materialIndex )const;
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
	};
}

#endif
