/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VolumesTraversal_H___
#define ___C3D_VolumesTraversal_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Volumetric/VolumetricModule.hpp"

namespace c3d
{
	C3D_API void createVolumesTraversalPass( crg::FramePassGroup & graph, RenderDevice const & device
		, Camera const & camera, VolumetricRendering const & volumetric, CameraUbo const & cameraUbo
		, Texture & transmittance, Texture & inscatter );
}

#endif
