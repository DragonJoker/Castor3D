/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPipeline_H___
#define ___C3D_LightPipeline_H___

#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/GpuBufferOffset.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/RunnablePasses/PipelineHolder.hpp>

namespace castor3d
{
	struct LightPipelineConfig
	{
		LightPipelineConfig( LightingModelID lightingModelId
			, SceneFlags const & sceneFlags
			, Light const & light );

		size_t makeHash()const;
		std::string getName( Engine const & engine );

		LightingModelID lightingModelId;
		SceneFlags const & sceneFlags;
		LightType lightType;
		ShadowType shadowType;
		bool shadows;
	};

	struct LightRenderPass
	{
		struct Entry
		{
			crg::ImageViewIdArray view;
			VkImageLayout input;
			VkImageLayout output;
		};

		struct Framebuffer
		{
			ashes::FrameBufferPtr fbo;
			std::vector< Entry > attaches;
		};

		ashes::RenderPassPtr renderPass;
		ashes::VkClearValueArray clearValues;
		std::vector< Framebuffer > framebuffers;
	};
	using LightRenderPassArray = std::vector< LightRenderPass >;

	uint32_t getLightRenderPassIndex( bool blend
		, LightType lightType );

	struct LightDescriptors
	{
		explicit LightDescriptors( Light const & light
			, RenderDevice const & device );

		Light const & light;
		MatrixUbo matrixUbo;
		UniformBufferOffsetT< ModelBufferConfiguration > modelMatrixUbo;
		ashes::DescriptorSetPtr descriptorSet;
		GpuBufferOffsetT< float > vertexBuffer;
		uint32_t angle;
	};

	class LightPipeline
	{
	public:
		LightPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, LightPipelineConfig const & config
			, LightRenderPassArray const & renderPasses
			, ashes::PipelineShaderStageCreateInfoArray stages
			, VkDescriptorSetLayout descriptorSetLayout
			, VkExtent2D const & targetExtent );

		VkPipeline getPipeline( uint32_t index )
		{
			return m_holder.getPipeline( index );
		}

		VkPipelineLayout getPipelineLayout()const
		{
			return m_holder.getPipelineLayout();
		}

		VkDescriptorSet getDescriptorSet()
		{
			return m_holder.getDescriptorSet( 0u );
		}

		LightType getLightType()const
		{
			return m_config.lightType;
		}

	protected:
		void doCreatePipeline( VkExtent2D const & targetExtent );

	private:
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState( VkExtent2D const & targetExtent );
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( bool blend );
		VkCullModeFlags doGetCullMode()const;

	private:
		crg::PipelineHolder m_holder;
		LightPipelineConfig const & m_config;
		LightRenderPassArray const & m_renderPasses;
	};
}

#endif
