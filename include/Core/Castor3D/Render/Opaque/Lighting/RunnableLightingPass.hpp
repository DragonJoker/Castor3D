/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RunnableLightingPass_H___
#define ___C3D_RunnableLightingPass_H___

#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Render/Opaque/Lighting/LightsPipeline.hpp"

namespace castor3d
{
	class RunnableLightingPass
		: public crg::RunnablePass
	{
	public:
		RunnableLightingPass( LightingPass const & lightingPass
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, Scene const & scene
			, LightPassResult const & lpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, crg::ImageId const & targetColourResult );

		void clear();
		void enableLight( Camera const & camera
			, Light const & light );
		void disableLight( Camera const & camera
			, Light const & light );
		void resetCommandBuffer();
		bool hasEnabledLights()const;
		uint32_t getEnabledLightsCount()const;

	protected:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		LightRenderPass doCreateRenderPass( bool blend
			, bool hasStencil
			, LightPassResult const & result );
		LightRenderPass doCreateStencilRenderPass( bool blend
			, LightPassResult const & result );
		LightsPipeline & doFindPipeline( Light const & light );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		LightPassResult const & m_lpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		crg::ImageId const & m_targetColourResult;
		std::vector< LightRenderPass > m_renderPasses;
		std::vector< LightRenderPass > m_stencilRenderPasses;
		std::map< size_t, LightsPipelinePtr > m_pipelines;
	};
}

#endif
