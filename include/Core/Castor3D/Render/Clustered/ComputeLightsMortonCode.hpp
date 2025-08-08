/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeLightsMortonCode_H___
#define ___C3D_ComputeLightsMortonCode_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API ClustersLightSortAttachs createComputeLightsMortonCodePass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, BufferBase const & allLightsAABB
		, BufferBase const & reducedLightsAABB
		, crg::BufferViewIdArray const & pointLightMortonCodes
		, crg::BufferViewIdArray const & spotLightMortonCodes
		, crg::BufferViewIdArray const & pointLightIndices
		, crg::BufferViewIdArray const & spotLightIndices );
}

#endif
