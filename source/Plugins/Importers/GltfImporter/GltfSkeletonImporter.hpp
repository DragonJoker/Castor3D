/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfSkeletonImporter___
#define ___C3D_GltfSkeletonImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Model/Skeleton/SkeletonImporter.hpp>

namespace c3d_gltf
{
	class GltfSkeletonImporter
		: public castor3d::SkeletonImporter
	{
	public:
		explicit GltfSkeletonImporter( castor3d::Engine & engine );

	private:
		bool doImportSkeleton( castor3d::Skeleton & node )override;
	};
}

#endif
