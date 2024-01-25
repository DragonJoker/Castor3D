/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpAnimationImporter___
#define ___C3D_AssimpAnimationImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Animation/AnimationImporter.hpp>

namespace c3d_assimp
{
	using SkeletonAnimationKeyFrameMap = castor::Map< castor::Milliseconds, castor3d::SkeletonAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = castor::Set< castor3d::SkeletonAnimationObjectRPtr >;

	class AssimpAnimationImporter
		: public castor3d::AnimationImporter
	{
	public:
		explicit AssimpAnimationImporter( castor3d::Engine & engine );

	private:
		bool doImportSkeleton( castor3d::SkeletonAnimation & animation )override;
		bool doImportMesh( castor3d::MeshAnimation & animation )override;
		bool doImportNode( castor3d::SceneNodeAnimation & animation )override;

		void doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
			, castor::Milliseconds maxTime
			, int64_t ticksPerSecond
			, castor3d::Skeleton const & skeleton
			, aiNode const & aiNode
			, aiAnimation const & aiAnimation
			, SkeletonAnimationKeyFrameMap & keyFrames
			, SkeletonAnimationObjectSet & notAnimated );
	};
}

#endif
