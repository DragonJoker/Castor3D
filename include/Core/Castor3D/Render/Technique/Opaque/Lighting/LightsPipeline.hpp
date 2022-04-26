/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightsPipeline_H___
#define ___C3D_LightsPipeline_H___

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPipeline.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

namespace castor3d
{
	struct LightsPipeline
	{
		LightsPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightPipelineConfig const & config
			, LightPassResult const & lpResult
			, ShadowMapResult const & smResult
			, std::vector< LightRenderPass > const & renderPasses );

		void clear();
		void addLight( Camera const & camera
			, Light const & light );
		void removeLight( Camera const & camera
			, Light const & light );
		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t & index );

	private:
		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout();
		LightDescriptors & doCreateLightEntry( Light const & light );
		castor::Matrix4x4f doComputeModelMatrix( castor3d::Light const & light
			, Camera const & camera )const;
		GpuBufferOffsetT< float > doCreateVertexBuffer();

	private:
		crg::GraphContext & m_context;
		ShadowMapResult const & m_smResult;
		RenderDevice const & m_device;
		std::vector< LightRenderPass > const & m_renderPasses;
		LightPipelineConfig m_config;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		LightPipeline m_pipeline;
		uint32_t m_count{};
		GpuBufferOffsetT< float > m_vertexBuffer;
		std::map< size_t, std::unique_ptr< LightDescriptors > > m_lightDescriptors;
		std::vector< LightDescriptors const * > m_enabledLights;
		Viewport m_viewport;
	};

	using LightsPipelinePtr = std::unique_ptr< LightsPipeline >;
}

#endif
