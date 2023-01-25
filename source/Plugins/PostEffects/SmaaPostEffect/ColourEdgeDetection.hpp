/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_ColourEdgeDetection_H___
#define ___C3DSMAA_ColourEdgeDetection_H___

#include "SmaaPostEffect/EdgeDetection.hpp"

namespace smaa
{
	class ColourEdgeDetection
		: public EdgeDetection
	{
	public:
		ColourEdgeDetection( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::ImageViewIdArray const & colourView
			, crg::ImageViewId const * predication
			, SmaaConfig const & config
			, bool const * enabled
			, uint32_t const * passIndex );

	private:
		crg::ImageViewId m_predicationView;
	};
}

#endif
