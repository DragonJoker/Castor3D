/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightsPipeline_H___
#define ___C3D_LightsPipeline_H___

#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPipeline.hpp"
#include "Castor3D/Render/Opaque/Lighting/StencilPipeline.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

namespace castor3d
{
	struct LightsPipeline
	{
		LightsPipeline( Scene const & scene
			, Camera const & camera
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightPipelineConfig const & config
			, LightPassResult const & lpResult
			, ShadowMapResult const & smResult
			, LightRenderPassArray const & renderPasses
			, LightRenderPassArray const & stencilRenderPasses
			, crg::ImageViewIdArray const & targetColourResult );

		void clear();
		void updateCamera( Camera const & camera );
		void addLight( Light const & light
			, ShadowBuffer const & shadowBuffer );
		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t passIndex
			, uint32_t & index );

		uint32_t getLightCount()const
		{
			return uint32_t( m_enabledLights.size() );
		}

	private:
		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout();
		LightDescriptors & doCreateLightEntry( Light const & light
			, ShadowBuffer const & shadowBuffer );
		castor::Matrix4x4f doComputeModelMatrix( castor3d::Light const & light
			, Camera const & camera )const;
		GpuBufferOffsetT< float > doCreateVertexBuffer( Light const * light );
		void doUpdateVertexBuffer( GpuBufferOffsetT< float > & vertexBuffer
			, uint32_t angle );
		void doRecordLightPassBase( LightRenderPass const & renderPass
			, VkDescriptorSet baseDS
			, size_t lightIndex
			, crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t passIndex
			, uint32_t pipelineIndex );
		void doRecordStencilPrePass( LightRenderPass const & renderPass
			, VkDescriptorSet baseDS
			, size_t lightIndex
			, crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t passIndex );
		void doRecordLightPass( LightRenderPass const & renderPass
			, LightRenderPass const & stencilRenderPass
			, VkDescriptorSet baseDS
			, size_t lightIndex
			, crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t passIndex
			, uint32_t pipelineIndex );

	private:
		crg::GraphContext & m_context;
		ShadowMapResult const & m_smResult;
		RenderDevice const & m_device;
		LightRenderPassArray const & m_renderPasses;
		Scene const & m_scene;
		Camera const & m_targetCamera;
		LightPipelineConfig m_config;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		LightPipeline m_lightPipeline;
		LightRenderPassArray const & m_stencilRenderPasses;
		StencilPipelinePtr m_stencilPipeline;
		uint32_t m_count{};
		GpuBufferOffsetT< float > m_vertexBuffer;
		std::map< size_t, std::unique_ptr< LightDescriptors > > m_lightDescriptors;
		std::vector< LightDescriptors const * > m_enabledLights;
		Viewport m_viewport;
		crg::ImageViewIdArray m_targetColourResult;
		Camera const * m_camera{};
	};

	using LightsPipelinePtr = std::unique_ptr< LightsPipeline >;
}

#endif
