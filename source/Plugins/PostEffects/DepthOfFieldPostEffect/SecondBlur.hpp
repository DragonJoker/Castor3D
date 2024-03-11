/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_SecondBlur_HPP___
#define ___C3DDFO_SecondBlur_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePassArray createSecondBlurPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, castor3d::Texture const & firstBlurResult
		, castor3d::Texture const & blurResult
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
