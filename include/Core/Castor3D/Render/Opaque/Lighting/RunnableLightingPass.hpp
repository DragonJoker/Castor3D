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
			, RenderTarget const & renderTarget
			, LightPassResult const & lpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, crg::ImageViewIdArray const & targetColourResult
			, crg::ImageViewIdArray const & targetDepthResult );

		void clear();
		void updateCamera( Camera const & camera );
		void enableLight( Light const & light );
		void resetCommandBuffer( crg::ImageViewId currentTarget );
		bool hasEnabledLights()const;
		uint32_t getEnabledLightsCount()const;

	protected:
		void doInitialise( uint32_t index );
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		using RunnablePass::resetCommandBuffer;
		using RunnablePass::resetCommandBuffers;

		LightRenderPass doCreateRenderPass( bool blend
			, bool hasStencil );
		LightRenderPass doCreateStencilRenderPass( bool blend );
		void doCreateFramebuffer( bool blend
			, bool hasStencil
			, LightRenderPass & renderPass
			, uint32_t index );
		void doCreateStencilFramebuffer( bool blend
			, LightRenderPass & renderPass
			, uint32_t index );
		LightsPipeline & doFindPipeline( Light const & light
			, LightingModelID lightingModelId );

	private:
		RenderDevice const & m_device;
		RenderTarget const & m_renderTarget;
		Scene const & m_scene;
		LightPassResult const & m_lpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		crg::ImageViewIdArray m_targetColourResult;
		crg::ImageViewIdArray m_targetDepthResult;
		crg::ImageViewId m_target;
		LightRenderPassArray m_renderPasses;
		LightRenderPassArray m_stencilRenderPasses;
		std::set< Light const * > m_pendingLights;
		std::map< size_t, LightsPipelinePtr > m_pipelines;
		uint32_t m_passIndex{};
	};
}

#endif
