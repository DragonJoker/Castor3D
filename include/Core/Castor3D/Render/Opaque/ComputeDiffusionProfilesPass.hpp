/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeDiffusionProfilesPass_H___
#define ___C3D_ComputeDiffusionProfilesPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <RenderGraph/RunnablePass.hpp>

namespace c3d
{
	C3D_API void createComputeDiffusionProfilesPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, SssProfileBuffer & buffer
		, Texture & result );
}

#endif
