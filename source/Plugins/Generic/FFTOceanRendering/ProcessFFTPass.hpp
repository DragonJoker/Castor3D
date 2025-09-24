/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_ProcessFFTPass_H___
#define ___C3DORFFT_ProcessFFTPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>

namespace ocean_fft
{
	void createProcessFFTPass( c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Extent2D const & extent
		, VkFFTConfig const & config
		, c3d::BufferBase const & input
		, c3d::Array< c3d::BufferUPtr, 2u > const & output );
}

#endif
