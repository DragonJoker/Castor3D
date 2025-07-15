/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpAnimationImporter___
#define ___C3D_AssimpAnimationImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Animation/AnimationImporter.hpp>

namespace c3d_assimp
{
	using SkeletonAnimationKeyFrameMap = c3d::Map< c3d::Milliseconds, c3d::SkeletonAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = c3d::Set< c3d::SkeletonAnimationObjectRPtr >;

	class AssimpAnimationImporter
		: public c3d::AnimationImporter
	{
	public:
		explicit AssimpAnimationImporter( c3d::Engine & engine );

	private:
		bool doImportSkeleton( c3d::SkeletonAnimation & animation )override;
		bool doImportMesh( c3d::MeshAnimation & animation )override;
		bool doImportNode( c3d::SceneNodeAnimation & animation )override;
		bool doImportTexture( c3d::TextureAnimation & animation )override;

		void doProcessSkeletonAnimationNodes( c3d::SkeletonAnimation & animation
			, c3d::Milliseconds minTime
			, c3d::Milliseconds maxTime
			, int64_t ticksPerSecond
			, c3d::Skeleton const & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated );
	};
}

#endif
