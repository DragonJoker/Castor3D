/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfAnimationImporter___
#define ___C3D_GltfAnimationImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Animation/AnimationImporter.hpp>

namespace c3d_gltf
{
	using SkeletonAnimationKeyFrameMap = castor::Map< castor::Milliseconds, castor3d::SkeletonAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = castor::Set< castor3d::SkeletonAnimationObjectRPtr >;

	class GltfAnimationImporter
		: public castor3d::AnimationImporter
	{
	public:
		explicit GltfAnimationImporter( castor3d::Engine & engine );

	private:
		bool doImportSkeleton( castor3d::SkeletonAnimation & animation )override;
		bool doImportMesh( castor3d::MeshAnimation & animation )override;
		bool doImportNode( castor3d::SceneNodeAnimation & animation )override;
	};
}

#endif
