/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AssignLightsToFroxels_H___
#define ___C3D_AssignLightsToFroxels_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

namespace c3d
{
	C3D_API void createAssignLightsToFroxelsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumFroxels const & froxels
		, FrustumClusters const & clusters
		, BufferBase & froxelsPointLightIndex
		, BufferBase & froxelsSpotLightIndex
		, BufferBase & counters
		, BufferBase & indirect );
}

#endif
