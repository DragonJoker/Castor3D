/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MergeSortLightsMortonCode_H___
#define ___C3D_MergeSortLightsMortonCode_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createMergeSortLightsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, ClustersLightSortAttachs const & inputSortAttachs
		, ClustersLightSortAttachs & outputSortAttachs
		, BufferBase & mergePathPartitions );
}

#endif
