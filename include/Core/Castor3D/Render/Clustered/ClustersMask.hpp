/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersMask_H___
#define ___C3D_ClustersMask_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	crg::FramePass const & createClustersMaskPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, RenderDevice const & device
		, CameraUbo const & clustersCameraUbo
		, FrustumClusters & clusters
		, RenderTechnique & technique
		, RenderNodesPass *& nodesPass );
}

#endif
