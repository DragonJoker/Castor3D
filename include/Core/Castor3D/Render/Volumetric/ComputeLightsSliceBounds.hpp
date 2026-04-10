/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeLightsSliceBounds_H___
#define ___C3D_ComputeLightsSliceBounds_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

namespace c3d
{
	C3D_API void createComputeLightsSliceBoundsPass( crg::FramePassGroup & graph, RenderDevice const & device, FrustumFroxels const & froxels
		, Texture const & finalFroxelsLighting, BufferBase & lightsSliceBounds );
}

#endif
