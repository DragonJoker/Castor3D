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
		DepthEdgeDetection( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & depthView
			, SmaaConfig const & config
			, bool const * enabled );

	private:
		crg::ImageViewId m_depthView;
		crg::ImageViewId const & m_sourceView;
	};
}

#endif
