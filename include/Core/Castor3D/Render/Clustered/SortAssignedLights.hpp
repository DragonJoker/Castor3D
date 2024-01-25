/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SortAssignedLights_H___
#define ___C3D_SortAssignedLights_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePass const & createSortAssignedLightsPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, FrustumClusters const & clusters );
}

#endif
