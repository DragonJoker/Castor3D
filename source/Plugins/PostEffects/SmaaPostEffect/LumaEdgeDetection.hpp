/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_LumaEdgeDetection_H___
#define ___C3DSMAA_LumaEdgeDetection_H___

#include "SmaaPostEffect/EdgeDetection.hpp"

namespace smaa
{
	class LumaEdgeDetection
		: public EdgeDetection
	{
	public:
		LumaEdgeDetection( crg::FramePassGroup & graph
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, c3d::Texture const & colourView
			, c3d::Texture const * predication
			, SmaaConfig const & config
			, bool const * enabled
			, uint32_t const * passIndex );
	};
}

#endif
