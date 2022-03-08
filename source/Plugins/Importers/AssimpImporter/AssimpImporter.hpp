/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp___
#define ___C3DAssimp___

#include <CastorUtils/FileParser/FileParser.hpp>

#include <Castor3D/Engine.hpp>

#include <Castor3D/Animation/Animation.hpp>
#include <Castor3D/Animation/AnimationKeyFrame.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#pragma warning( pop )

namespace C3dAssimp
{
	using SkeletonAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SkeletonAnimationKeyFrameUPtr >;
	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = std::set< castor3d::SkeletonAnimationObjectSPtr >;

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

	class AssimpImporter
		: public castor3d::MeshImporter
	{
	public:
		explicit AssimpImporter( castor3d::Engine & engine );

		static castor3d::MeshImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & mesh )override;
		bool doImportScene( castor3d::Scene & scene )override;

		void doProcessLight( aiLight const & light
			, castor3d::Scene & scene );
		void doProcessSceneNodes( aiScene const & aiScene
			, aiNode const & aiNode
			, castor3d::Scene & scene
			, castor3d::SceneNodeSPtr targetParent
			, castor::Matrix4x4f accTransform );
		void doProcessNodes( aiScene const & aiScene
			, aiNode const & aiNode
			, castor3d::Scene & scene
			, std::map< uint32_t, MeshData * > const & meshes );
		aiScene const * doLoadScene();
		std::vector< MeshData > doProcessMeshesAndAnims( aiScene const & aiScene
			, castor3d::Scene & scene
			, std::vector< aiMesh * > const & aiMeshes
			, castor3d::MeshPtrStrMap & registeredMeshes );
		std::map< uint32_t, MeshData * > doMergeMeshes( castor3d::Scene & scene
			, std::vector< MeshData > & source );
		bool doProcessMeshAndAnims( aiScene const & aiScene
			, std::vector< aiMesh * > aiMeshes
			, castor3d::Mesh & mesh );
		bool doProcessMesh( castor3d::Scene & scene
			, castor3d::Mesh & mesh
			, castor3d::Skeleton & skeleton
			, aiMesh const & aiMesh
			, uint32_t aiMeshIndex
			, aiScene const & aiScene
			, castor3d::Submesh & submesh );
		castor3d::MaterialResPtr doProcessMaterial( castor3d::Scene & scene
			, aiScene const & aiScene
			, aiMaterial const & aiMaterial
			, uint32_t index );
		castor3d::BoneSPtr doAddBone( castor::String const & name
			, castor::Matrix4x4f const & offset
			, castor3d::Skeleton & skeleton
			, uint32_t & index );
		void doProcessBones( castor3d::Skeleton & pSkeleton
			, aiBone const * const * aiBones
			, uint32_t count
			, std::vector< castor3d::VertexBoneData > & arrayVertices );
		void doProcessAnimation( castor3d::Mesh & mesh
			, castor::String const & name
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation );
		void doProcessAnimationNodes( castor3d::Mesh & mesh
			, castor3d::SkeletonAnimation & animation
			, int64_t ticksPerSecond
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation
			, castor3d::SkeletonAnimationObjectSPtr object
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated );
		void doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
			, int64_t ticksPerSecond
			, castor3d::SkeletonAnimationObject & object
			, castor3d::SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes );
		void doProcessAnimationMeshes( castor3d::Mesh & mesh
			, castor3d::Submesh & submesh
			, aiMesh const & aiMesh
			, aiAnimMesh const & aiAnimMesh
			, uint32_t index );
		void doProcessAnimationSceneNodes( castor3d::SceneNode & node
			, aiAnimation const & aiAnimation
			, aiNode const & aiNode
			, aiNodeAnim const & aiNodeAnim );

	public:
		static castor::String const Name;

	private:
		int m_anonymous;
		Assimp::Importer m_importer;
		uint32_t m_flags{};
		std::map< castor::String, uint32_t > m_mapBoneByID;
		std::vector< castor3d::BoneSPtr > m_arrayBones;
		std::map< uint32_t, uint32_t > m_submeshByID;
	};
}

#endif
