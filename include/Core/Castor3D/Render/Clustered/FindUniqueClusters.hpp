/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FindUniqueClusters_H___
#define ___C3D_FindUniqueClusters_H___

#include "ClusteredModule.hpp"

namespace c3d
{
	C3D_API void createFindUniqueClustersPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, BufferBase const & clusterFlags
		, BufferBase & uniqueClusters
		, BufferBase & clustersIndirect );
}

#endif
