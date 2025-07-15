/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_DepthEdgeDetection_H___
#define ___C3DSMAA_DepthEdgeDetection_H___

#include "SmaaPostEffect/EdgeDetection.hpp"

namespace smaa
{
	class DepthEdgeDetection
		: public EdgeDetection
	{
	public:
		DepthEdgeDetection( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::ImageViewId const & depthObj
			, SmaaConfig const & config
			, bool const * enabled );
	};
}

#endif
