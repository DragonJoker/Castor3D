/*
See LICENSE file in root folder
*/
#ifndef ___C3DAssimp_SkeletonImporter___
#define ___C3DAssimp_SkeletonImporter___

#include <Castor3D/Castor3DPrerequisites.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/scene.h> // Output data structure
#pragma warning( pop )

namespace c3d_assimp
{
	using SkeletonAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SkeletonAnimationKeyFrameUPtr >;
	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = std::set< castor3d::SkeletonAnimationObjectSPtr >;

	class SkeletonImporter
	{
	public:
		explicit SkeletonImporter( castor3d::MeshImporter & importer );

		void import( castor::String const & prefix
			, castor::Path const & fileName
			, uint32_t importFlags
			, aiScene const & aiScene
			, castor3d::Scene & scene );
		bool isBoneNode( aiNode const & aiNode )const;
		castor3d::SkeletonSPtr getSkeleton( aiMesh const & aiMesh )const;

	private:
		bool doCheckNewSkeleton( castor3d::SkeletonSPtr & skeleton );
		void doProcessSkeletons( aiScene const & aiScene
			, castor3d::Scene & scene
			, castor::ArrayView< aiMesh * > aiMeshes );
		void doProcessBones( castor3d::Skeleton & skeleton
			, castor::ArrayView< aiBone * > aiBones );
		void doLinkSkeletonNodes( castor3d::Skeleton & skeleton
			, aiNode const & aiNode );
		void doProcessSkeletonAnimation( castor::String const & name
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation );
		void doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
			, int64_t ticksPerSecond
			, castor3d::Skeleton & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation
			, castor3d::SkeletonAnimationObjectSPtr object
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated );
		void doProcessSkeletonNodes( castor3d::Skeleton & skeleton
			, aiScene const & aiScene
			, aiNode const & aiNode );
		void doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
			, int64_t ticksPerSecond
			, castor3d::SkeletonAnimationObject & object
			, castor3d::SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes );

	private:
		castor3d::MeshImporter & m_importer;
		castor::String m_prefix;
		castor::Path m_fileName;
		uint32_t m_importFlags{};
		std::map< aiNode const *, aiBone const * > m_bonesNodes;
		std::set< castor3d::SkeletonSPtr > m_skeletons;
		std::map< aiMesh const *, castor3d::SkeletonSPtr > m_meshSkeletons;
	};
}

#endif
