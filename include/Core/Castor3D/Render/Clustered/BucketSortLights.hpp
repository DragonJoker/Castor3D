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
		, crg::BufferViewIdArray const & outPointLightMortonCodes
		, crg::BufferViewIdArray const & outSpotLightMortonCodes
		, crg::BufferViewIdArray const & outPointLightIndices
		, crg::BufferViewIdArray const & outSpotLightIndices );
}

#endif
