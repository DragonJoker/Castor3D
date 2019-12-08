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
#include <Castor3D/Material/Pass.hpp>
#include <Castor3D/Mesh/SubmeshComponent/Face.hpp>
#include <Castor3D/Mesh/Importer.hpp>
#include <Castor3D/Mesh/Mesh.hpp>
#include <Castor3D/Mesh/Submesh.hpp>
#include <Castor3D/Mesh/Vertex.hpp>
#include <Castor3D/Mesh/Skeleton/Skeleton.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>
#include <Castor3D/Plugin/Plugin.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>
#include <Castor3D/Texture/TextureLayout.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags

namespace C3dAssimp
{
	using SkeletonAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SkeletonAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = std::set< castor3d::SkeletonAnimationObjectSPtr >;
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\~english
	\brief		ASE file importer
	\~french
	\brief		Importeur de fichiers ASE
	*/
	class AssimpImporter
		: public castor3d::Importer
	{
	public:
		explicit AssimpImporter( castor3d::Engine & engine );
		~AssimpImporter();

		static castor3d::ImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;

		bool doProcessMesh( castor3d::Scene & p_scene
			, castor3d::Mesh & p_mesh
			, castor3d::Skeleton & p_skeleton
			, aiMesh const & p_aiMesh
			, aiScene const & p_aiScene
			, castor3d::Submesh & p_submesh );
		castor3d::MaterialSPtr doProcessMaterial( castor3d::Scene & p_scene
			, aiMaterial const & p_aiMaterial );
		castor3d::BoneSPtr doAddBone( castor::String const & p_name
			, castor::Matrix4x4f const & p_offset
			, castor3d::Skeleton & p_skeleton
			, uint32_t & p_index );
		void doProcessBones( castor3d::Skeleton & p_pSkeleton
			, aiBone const * const * p_aiBones
			, uint32_t p_count
			, std::vector< castor3d::VertexBoneData > & p_arrayVertices );
		void doProcessAnimation( castor3d::Mesh & p_mesh
			, castor::String const & p_name
			, castor3d::Skeleton & p_skeleton
			, aiNode const & p_aiNode
			, aiAnimation const & p_aiAnimation );
		void doProcessAnimationNodes( castor3d::Mesh & p_mesh
			, castor3d::SkeletonAnimation & p_animation
			, int64_t p_ticksPerMilliSecond
			, castor3d::Skeleton & p_skeleton
			, aiNode const & p_aiNode
			, aiAnimation const & p_aiAnimation
			, castor3d::SkeletonAnimationObjectSPtr p_object
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated );
		void doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
			, int64_t ticksPerMilliSecond
			, castor3d::SkeletonAnimationObject & object
			, castor3d::SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes );
		void doProcessAnimationMeshes( castor3d::Mesh & p_mesh
			, castor3d::Submesh & p_submesh
			, aiMesh const & p_aiMesh
			, aiMeshAnim const & p_aiMeshAnim );

	public:
		static castor::String const Name;

	private:
		int m_anonymous;
		std::map< castor::String, uint32_t > m_mapBoneByID;
		std::vector< castor3d::BoneSPtr > m_arrayBones;
	};
}

#endif
