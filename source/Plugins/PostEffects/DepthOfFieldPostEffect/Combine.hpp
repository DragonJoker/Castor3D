/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_Combine_HPP___
#define ___C3DDFO_Combine_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	void createCombinePass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & nearBlur
		, c3d::Texture const & farBlur
		, c3d::Texture const & source
		, c3d::Texture & target
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
