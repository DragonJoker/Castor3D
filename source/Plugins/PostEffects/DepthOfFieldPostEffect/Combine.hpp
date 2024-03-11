/*
See LICENSE file in root folder
*/
#ifndef ___C3DDFO_Combine_HPP___
#define ___C3DDFO_Combine_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace dof
{
	class DepthOfFieldUbo;

	crg::FramePass const & createCombinePass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & nearBlur
		, castor3d::Texture const & farBlur
		, crg::ImageViewIdArray const & target
		, bool const * enabled
		, uint32_t const * passIndex );
}

#endif
