/*
See LICENSE file in root folder
*/
#ifndef ___C3DDOF_ComputeCircleOfConfusion_HPP___
#define ___C3DDOF_ComputeCircleOfConfusion_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	void createComputeCircleOfConfusionPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & depth
		, c3d::Texture const & colour
		, c3d::Texture & nearCoC
		, c3d::Texture & farCoC
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
