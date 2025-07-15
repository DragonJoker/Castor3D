/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_SecondBlur_HPP___
#define ___C3DDFO_SecondBlur_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePassArray createSecondBlurPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & firstBlurResult
		, c3d::Texture const & blurResult
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, uint32_t const * passIndex );
}

#endif
