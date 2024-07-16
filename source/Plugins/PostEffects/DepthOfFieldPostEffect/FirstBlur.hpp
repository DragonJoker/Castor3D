/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_FirstBlur_HPP___
#define ___C3DDFO_FirstBlur_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePassArray createFirstBlurPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & cocResult
		, castor3d::Texture const & blurResult
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, uint32_t const * passIndex );
}

#endif
