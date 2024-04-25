/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReduceLightsAABB_H___
#define ___C3D_ReduceLightsAABB_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePass const & createReduceLightsAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, FrustumClusters & clusters );
}

#endif
