/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersMask_H___
#define ___C3D_ClustersMask_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	void createClustersMaskPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & reducedLightsAABB
		, RenderTechnique & technique
		, RenderNodesPass *& nodesPass );
}

#endif
