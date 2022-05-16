/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssimpSkeletonImporter___
#define ___C3D_AssimpSkeletonImporter___

#include "AssimpImporter/AssimpImporterFile.hpp"

#include <Castor3D/Model/Skeleton/SkeletonImporter.hpp>

namespace c3d_assimp
{
	class AssimpSkeletonImporter
		: public castor3d::SkeletonImporter
	{
	public:
		explicit AssimpSkeletonImporter( castor3d::Engine & engine );

	private:
		bool doImportSkeleton( castor3d::Skeleton & node )override;
	};
}

#endif
