/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StencilPipeline_H___
#define ___C3D_StencilPipeline_H___

#include "Castor3D/Render/Technique/Opaque/Lighting/LightPipeline.hpp"

namespace castor3d
{
	class StencilPipeline
	{
	public:
		StencilPipeline( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, LightPipelineConfig const & config
			, std::vector< LightRenderPass > const & renderPasses
			, VkDescriptorSetLayout descriptorSetLayout );

		VkPipeline getPipeline()
		{
			return m_holder.getPipeline( 0u );
		}

		VkPipelineLayout getPipelineLayout()const
		{
			return m_holder.getPipelineLayout();
		}

		VkDescriptorSet getDescriptorSet()
		{
			return m_holder.getDescriptorSet( 0u );
		}

	protected:
		void doCreatePipeline();

	private:
		ashes::PipelineVertexInputStateCreateInfo doCreateVertexLayout();
		ashes::PipelineViewportStateCreateInfo doCreateViewportState( ashes::FrameBuffer const & framebuffer );

	private:
		ShaderModule m_vertexShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::PipelineHolder m_holder;
		std::vector< LightRenderPass > const & m_renderPasses;
	};

	using StencilPipelinePtr = std::unique_ptr< StencilPipeline >;
}

#endif
