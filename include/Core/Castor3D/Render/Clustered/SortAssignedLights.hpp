/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SortAssignedLights_H___
#define ___C3D_SortAssignedLights_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createSortAssignedLightsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, PagedBufferBase & pointLightClusterIndex
		, PagedBufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid );
}

#endif
