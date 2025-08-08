/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssignLightsToClusters_H___
#define ___C3D_AssignLightsToClusters_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & allLightsAABB
		, BufferBase const & clustersAABB
		, BufferBase const & pointLightBVH
		, BufferBase const & spotLightBVH
		, ClustersLightSortAttachs const & outputSortAttachs
		, BufferBase & pointLightClusterIndex
		, BufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid );
}

#endif
