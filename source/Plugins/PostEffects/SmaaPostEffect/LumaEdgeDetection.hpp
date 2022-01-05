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
			, crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::ImageViewId const & colourView
			, crg::ImageViewId const * predication
			, SmaaConfig const & config
			, bool const * enabled );

	private:
		crg::ImageViewId const & m_colourView;
		crg::ImageViewId m_predicationView;
	};
}

#endif
