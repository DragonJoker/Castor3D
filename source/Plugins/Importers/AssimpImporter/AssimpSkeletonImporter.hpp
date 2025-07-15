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
		: public c3d::SkeletonImporter
	{
	public:
		explicit AssimpSkeletonImporter( c3d::Engine & engine );

	private:
		bool doImportSkeleton( c3d::Skeleton & node )override;
	};
}

#endif
