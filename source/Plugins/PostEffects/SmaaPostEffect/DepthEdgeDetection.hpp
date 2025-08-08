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
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, c3d::Texture const & depthObj
			, SmaaConfig const & config
			, bool const * enabled );
	};
}

#endif
