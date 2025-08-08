/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_GenerateMipmapsPass_H___
#define ___C3DORFFT_GenerateMipmapsPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

namespace ocean_fft
{
	void createGenerateSpecMipmapsPass( c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Texture & imageView );
}

#endif
