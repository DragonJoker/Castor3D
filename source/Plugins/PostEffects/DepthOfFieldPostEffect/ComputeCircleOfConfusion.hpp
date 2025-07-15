/*
See LICENSE file in root folder
*/
#ifndef ___C3DDOF_ComputeCircleOfConfusion_HPP___
#define ___C3DDOF_ComputeCircleOfConfusion_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePassArray createComputeCircleOfConfusionPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & depth
		, crg::ImageViewIdArray const & colour
		, c3d::Texture const & nearCoC
		, c3d::Texture const & farCoC
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
