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
		: public c3d::SkeletonImporter
	{
	public:
		explicit GltfSkeletonImporter( c3d::Engine & engine );

	private:
		bool doImportSkeleton( c3d::Skeleton & node )override;
	};

	c3d::Vector< size_t > findSkinRootNodes( GltfImporterFile const & file
		, fastgltf::Skin const & impSkin );
}

#endif
