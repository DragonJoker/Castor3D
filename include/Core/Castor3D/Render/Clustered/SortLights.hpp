/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SortLights_H___
#define ___C3D_SortLights_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createSortLightsPasses( crg::FramePassGroup & graph
		, RenderDevice const & device
		, crg::ResourcesCache & resources
		, FrustumClusters & clusters
		, PagedBufferBase const & allLightsAABB
		, BufferBase const & reducedLightsAABB
		, BufferBase & pointLightIndices
		, BufferBase & spotLightIndices );
}

#endif
