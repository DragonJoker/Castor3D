/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPipeline_H___
#define ___C3D_LightPipeline_H___

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

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
		LightPipelineConfig( PassTypeID passType
			, SceneFlags const & sceneFlags
			, Light const & light );

		size_t makeHash()const;

		PassTypeID passType;
		SceneFlags const & sceneFlags;
		LightType lightType;
		ShadowType shadowType;
		bool shadows;
	};

	struct LightRenderPass
	{
		ashes::RenderPassPtr renderPass;
		ashes::FrameBufferPtr framebuffer;
		ashes::VkClearValueArray clearValues;
		struct Entry
		{
			crg::ImageViewId view;
			VkImageLayout input;
			VkImageLayout output;
		};
		std::vector< Entry > attaches;
	};

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
	};

	class LightPipeline
	{
	public:
		LightPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, LightPipelineConfig const & config
			, std::vector< LightRenderPass > const & renderPasses
			, ashes::PipelineShaderStageCreateInfoArray stages
			, VkDescriptorSetLayout descriptorSetLayout );

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
		void doCreatePipeline();

	private:
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState( ashes::FrameBuffer const & framebuffer );
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( bool blend );
		VkCullModeFlags doGetCullMode()const;

	private:
		crg::PipelineHolder m_holder;
		LightPipelineConfig const & m_config;
		std::vector< LightRenderPass > const & m_renderPasses;
	};
}

#endif
