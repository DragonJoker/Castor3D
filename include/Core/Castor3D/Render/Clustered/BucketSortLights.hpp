/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BucketSortLightsMortonCode_H___
#define ___C3D_BucketSortLightsMortonCode_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API ClustersLightSortAttachs createBucketSortLightsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, ClustersLightSortAttachs const & sortAttachs
		, crg::BufferViewIdArray const & inputPointLightMortonCodes
		, crg::BufferViewIdArray const & inputSpotLightMortonCodes
		, crg::BufferViewIdArray const & inputPointLightIndices
		, crg::BufferViewIdArray const & inputSpotLightIndices );
}

#endif
