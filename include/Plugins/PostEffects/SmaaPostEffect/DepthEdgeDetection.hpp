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
		DepthEdgeDetection( castor3d::RenderTarget & renderTarget
			, ashes::ImageView const & depthView
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex )override;

	private:
		virtual void doInitialisePipeline()override;

	private:
		ashes::ImageView m_depthView;
		ashes::ImageView const & m_sourceView;
	};
}

#endif
