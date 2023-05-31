/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClusteredLightsPipeline_H___
#define ___C3D_ClusteredLightsPipeline_H___

#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPipeline.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>

namespace castor3d
{
	struct ClusteredLightsPipeline
	{
		ClusteredLightsPipeline( Scene const & scene
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, FrustumClusters const & frustumClusters
			, LightPipelineConfig const & config
			, LightPassResult const & lpResult
			, LightRenderPassArray const & renderPasses
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer
			, crg::ImageViewIdArray const & targetColourResult );

		void clear();
		void updateCamera( Camera const & camera );
		void addLight( Light const & light );
		void recordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, u32 passIndex
			, u32 & index );

		u32 getLightCount()const
		{
			return m_lightsCount;
		}

	private:
		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout();
		ashes::DescriptorSetPtr doCreateDescriptorSet( crg::RunnableGraph & graph
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer );
		GpuBufferOffsetT< float > doCreateVertexBuffer();

	private:
		crg::GraphContext & m_context;
		RenderDevice const & m_device;
		LightRenderPassArray const & m_renderPasses;
		Scene const & m_scene;
		FrustumClusters const & m_frustumClusters;
		crg::ImageViewIdArray m_targetColourResult;
		CameraUbo m_cameraUbo;
		LightPipelineConfig m_config;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		LightPipeline m_lightPipeline;
		u32 m_count{};
		GpuBufferOffsetT< float > m_vertexBuffer;
		Viewport m_viewport;
		u32 m_lightsCount{};
	};

	using ClusteredLightsPipelinePtr = std::unique_ptr< ClusteredLightsPipeline >;
}

#endif
