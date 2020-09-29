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
		ColourEdgeDetection( castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, ashes::ImageView const & colourView
			, ashes::Image const * predication
			, SmaaConfig const & config );
		castor3d::CommandsSemaphore prepareCommands( castor3d::RenderPassTimer const & timer
			, uint32_t passIndex )override;

	private:
		virtual void doInitialisePipeline()override;

	private:
		void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	private:
		ashes::ImageView const & m_colourView;
		std::unique_ptr< ashes::ImageView > m_predicationView;
	};
}

#endif
