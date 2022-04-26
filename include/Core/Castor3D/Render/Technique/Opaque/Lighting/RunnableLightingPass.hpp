/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RunnableLightingPass_H___
#define ___C3D_RunnableLightingPass_H___

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightsPipeline.hpp"

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
			, ShadowMapResult const & smSpotResult );

		void clear();
		void enableLight( Camera const & camera
			, Light const & light );
		void disableLight( Camera const & camera
			, Light const & light );
		void resetCommandBuffer();
		bool hasEnabledLights()const;

	protected:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );

	private:
		LightRenderPass doCreateRenderPass( bool blend
			, LightPassResult const & result );
		LightsPipeline & doFindPipeline( Light const & light );

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		LightPassResult const & m_lpResult;
		ShadowMapResult const & m_smDirectionalResult;
		ShadowMapResult const & m_smPointResult;
		ShadowMapResult const & m_smSpotResult;
		std::vector< LightRenderPass > m_renderPasses;
		std::map< size_t, LightsPipelinePtr > m_pipelines;
	};
}

#endif
