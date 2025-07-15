/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfAnimationImporter___
#define ___C3D_GltfAnimationImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Animation/AnimationImporter.hpp>

namespace c3d_gltf
{
	using SkeletonAnimationKeyFrameMap = c3d::Map< c3d::Milliseconds, c3d::SkeletonAnimationKeyFrameUPtr >;
	using SkeletonAnimationObjectSet = c3d::Set< c3d::SkeletonAnimationObjectRPtr >;

	class GltfAnimationImporter
		: public c3d::AnimationImporter
	{
	public:
		explicit GltfAnimationImporter( c3d::Engine & engine );

	private:
		bool doImportSkeleton( c3d::SkeletonAnimation & animation )override;
		bool doImportMesh( c3d::MeshAnimation & animation )override;
		bool doImportNode( c3d::SceneNodeAnimation & animation )override;
		bool doImportTexture( c3d::TextureAnimation & animation )override;
	};
}

#endif
