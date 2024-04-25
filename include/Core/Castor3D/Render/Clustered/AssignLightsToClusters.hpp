/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssignLightsToClusters_H___
#define ___C3D_AssignLightsToClusters_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePass const & createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, FrustumClusters & clusters );
}

#endif
