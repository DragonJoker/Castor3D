/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ReduceLightsAABB_H___
#define ___C3D_ReduceLightsAABB_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createReduceLightsAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & allLightsAABB
		, BufferBase & reducedLightsAABB );
}

#endif
