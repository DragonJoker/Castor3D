/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BucketSortLightsMortonCode_H___
#define ___C3D_BucketSortLightsMortonCode_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePassArray createBucketSortLightsPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, FrustumClusters & clusters );
}

#endif
