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
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] program
		*	Le programme shader.
		*\param[in] vertexBuffers
		*	Les tampons de sommets utilisés.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*/
		Pipeline( Device const & device
			, renderer::PipelineLayout const & layout
			, renderer::ShaderProgram const & program
			, renderer::VertexLayoutCRefArray const & vertexLayouts
			, renderer::RenderPass const & renderPass
			, renderer::PrimitiveTopology topology
			, renderer::RasterisationState const & rasterisationState
			, renderer::ColourBlendState const & colourBlendState );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Pipeline();
		/**
		*\brief
		*	Crée le pipeline.
		*/
		renderer::Pipeline & finish()override;
		/**
		*\brief
		*	Définit le MultisampleState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		renderer::Pipeline & multisampleState( renderer::MultisampleState const & state )override;
		/**
		*\brief
		*	Définit le DepthStencilState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		renderer::Pipeline & depthStencilState( renderer::DepthStencilState const & state )override;
		/**
		*\brief
		*	Définit le TessellationState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		renderer::Pipeline & tessellationState( renderer::TessellationState const & state )override;
		/**
		*\brief
		*	Définit le Viewport.
		*\param[in] viewport
		*	La nouvelle valeur.
		*/
		renderer::Pipeline & viewport( renderer::Viewport const & viewport )override;
		/**
		*\brief
		*	Définit le Scissor.
		*\param[in] scissor
		*	La nouvelle valeur.
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
		inline operator VkPipeline const &( )const
		{
			return m_pipeline;
		}

	private:
		Device const & m_device;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_shader;
		VertexLayoutCRefArray m_vertexLayouts;
		RenderPass const & m_renderPass;
		VkPrimitiveTopology m_topology;
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
