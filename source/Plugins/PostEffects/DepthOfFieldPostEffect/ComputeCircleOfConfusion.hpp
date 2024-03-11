/*
See LICENSE file in root folder
*/
#ifndef ___C3DDOF_ComputeCircleOfConfusion_HPP___
#define ___C3DDOF_ComputeCircleOfConfusion_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePassArray createComputeCircleOfConfusionPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, castor3d::Texture const & depth
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & nearCoC
		, castor3d::Texture const & farCoC
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
