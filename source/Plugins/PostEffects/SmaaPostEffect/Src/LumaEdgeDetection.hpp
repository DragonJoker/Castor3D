/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_LumaEdgeDetection_H___
#define ___C3DSMAA_LumaEdgeDetection_H___

#include "EdgeDetection.hpp"

namespace smaa
{
	class LumaEdgeDetection
		: public EdgeDetection
	{
	public:
		LumaEdgeDetection( castor3d::RenderTarget & renderTarget
			, renderer::TextureView const & colourView
			, renderer::Texture const * predication
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex )override;

	private:
		virtual void doInitialisePipeline()override;

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	private:
		renderer::TextureView const & m_colourView;
		renderer::TextureViewPtr m_predicationView;
	};
}

#endif
