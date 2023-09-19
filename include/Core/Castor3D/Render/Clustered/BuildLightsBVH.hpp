/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BuildLightsBVH_H___
#define ___C3D_BuildLightsBVH_H___

#include "ClusteredModule.hpp"

namespace castor3d
{
	C3D_API crg::FramePassArray createBuildLightsBVHPass( crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, RenderDevice const & device
		, FrustumClusters & clusters );
}

#endif
