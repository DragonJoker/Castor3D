/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssignLightsToClusters_H___
#define ___C3D_AssignLightsToClusters_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePass const & createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, LightCache const & lights
		, CameraUbo const & cameraUbo
		, FrustumClusters const & clusters );
}

#endif
