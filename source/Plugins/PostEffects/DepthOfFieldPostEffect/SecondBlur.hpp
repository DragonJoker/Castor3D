/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_SecondBlur_HPP___
#define ___C3DDFO_SecondBlur_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	void createSecondBlurPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & firstBlurResult
		, c3d::Texture & blurResult
		, crg::RunnablePass::IsEnabledCallback const & isEnabled
		, uint32_t const * passIndex );
}

#endif
