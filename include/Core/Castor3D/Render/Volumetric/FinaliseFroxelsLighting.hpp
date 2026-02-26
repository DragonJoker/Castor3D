/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FinaliseFroxelsLighting_H___
#define ___C3D_FinaliseFroxelsLighting_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

namespace c3d
{
	C3D_API void createFinaliseFroxelsLightingPass( crg::FramePassGroup & graph, RenderDevice const & device, FrustumFroxels const & froxels
		, Texture const & rawFroxelsLightingU32R, Texture const & rawFroxelsLightingU32G, Texture const & rawFroxelsLightingU32B
		, Texture & rawFroxelsLighting, Texture & finalFroxelsLightingU32G );
}

#endif
