/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_DepthEdgeDetection_H___
#define ___C3DSMAA_DepthEdgeDetection_H___

#include "EdgeDetection.hpp"

namespace smaa
{
	class DepthEdgeDetection
		: public EdgeDetection
	{
	public:
		DepthEdgeDetection( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & depthView
			, castor3d::SamplerSPtr sampler
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex )override;

	private:
		virtual void doInitialisePipeline()override;

	private:
		renderer::TextureView const & m_depthView;
	};
}

#endif
