/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Pipeline_HPP___
#define ___VkRenderer_Pipeline_HPP___
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Pipeline/Pipeline.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Un pipeline de rendu.
	*/
	class Pipeline
		: public renderer::Pipeline
	{
	public:
		/**
		*name
		*	Construction / Destruction.
		*/
		/**@{*/
		Pipeline( Device const & device
			, renderer::PipelineLayout const & layout
			, renderer::ShaderProgram const & program
			, renderer::VertexLayoutCRefArray const & vertexLayouts
			, renderer::RenderPass const & renderPass
			, renderer::InputAssemblyState const & inputAssemblyState
			, renderer::RasterisationState const & rasterisationState
			, renderer::ColourBlendState const & colourBlendState );
		~Pipeline();
		/**@}*/
		/**
		*\copydoc	renderer::Pipeline::finish
		*/
		renderer::Pipeline & finish()override;
		/**
		*\copydoc	renderer::Pipeline::multisampleState
		*/
		renderer::Pipeline & multisampleState( renderer::MultisampleState const & state )override;
		/**
		*\copydoc	renderer::Pipeline::depthStencilState
		*/
		renderer::Pipeline & depthStencilState( renderer::DepthStencilState const & state )override;
		/**
		*\copydoc	renderer::Pipeline::tessellationState
		*/
		renderer::Pipeline & tessellationState( renderer::TessellationState const & state )override;
		/**
		*\copydoc	renderer::Pipeline::viewport
		*/
		renderer::Pipeline & viewport( renderer::Viewport const & viewport )override;
		/**
		*\copydoc	renderer::Pipeline::scissor
		*/
		renderer::Pipeline & scissor( renderer::Scissor const & scissor )override;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkPipeline.
		*\~english
		*\brief
		*	VkPipeline implicit cast operator.
		*/
		inline operator VkPipeline const &()const
		{
			return m_pipeline;
		}

	private:
		Device const & m_device;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_shader;
		VertexLayoutCRefArray m_vertexLayouts;
		RenderPass const & m_renderPass;
		VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyState;
		VkPipelineRasterizationStateCreateInfo m_rasterisationState;
		std::vector< VkPipelineColorBlendAttachmentState > m_colourBlendStateAttachments;
		VkPipelineColorBlendStateCreateInfo m_colourBlendState;
		std::unique_ptr< VkViewport > m_viewport;
		std::unique_ptr< VkRect2D > m_scissor;
		std::unique_ptr< VkPipelineMultisampleStateCreateInfo > m_multisampleState;
		std::unique_ptr< VkPipelineDepthStencilStateCreateInfo > m_depthStencilState;
		std::unique_ptr< VkPipelineTessellationStateCreateInfo > m_tessellationState;
		VkPipeline m_pipeline{ VK_NULL_HANDLE };
	};
}

#endif
