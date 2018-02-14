/**
*\file
*	Texture.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Pipeline_HPP___
#define ___VkRenderer_Pipeline_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Pipeline/Pipeline.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/TessellationState.hpp>
#include <Pipeline/Viewport.hpp>

namespace gl_renderer
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
		*\return
		*	\p true si le Viewport est défini.
		*/
		inline bool hasViewport()const
		{
			return m_viewport != nullptr;
		}
		/**
		*\return
		*	\p true si le Scissor est défini.
		*/
		inline bool hasScissor()const
		{
			return m_scissor != nullptr;
		}
		/**
		*\return
		*	Le ColourBlendState.
		*/
		inline renderer::ColourBlendState const & getColourBlendState()const
		{
			return m_cbState;
		}
		/**
		*\return
		*	Le RasterisationState.
		*/
		inline renderer::RasterisationState const & getRasterisationState()const
		{
			return m_rsState;
		}
		/**
		*\return
		*	Le DepthStencilState.
		*/
		inline renderer::DepthStencilState const & getDepthStencilState()const
		{
			return m_dsState;
		}
		/**
		*\return
		*	Le MultisampleState.
		*/
		inline renderer::MultisampleState const & getMultisampleState()const
		{
			return m_msState;
		}
		/**
		*\return
		*	Le TessellationState.
		*/
		inline renderer::TessellationState const & getTessellationState()const
		{
			return m_tsState;
		}
		/**
		*\return
		*	Le Viewport.
		*/
		inline renderer::Viewport const & getViewport()const
		{
			assert( m_viewport );
			return *m_viewport;
		}
		/**
		*\return
		*	Le Scissor.
		*/
		inline renderer::Scissor const & getScissor()const
		{
			assert( m_scissor );
			return *m_scissor;
		}
		/**
		*\return
		*	Le PipelineLayout.
		*/
		inline renderer::PipelineLayout const & getLayout()const
		{
			return m_layout;
		}
		/**
		*\return
		*	Le ShaderProgram.
		*/
		inline ShaderProgram const & getProgram()const
		{
			return m_program;
		}

	private:
		Device const & m_device;
		renderer::PipelineLayout const & m_layout;
		ShaderProgram const & m_program;
		renderer::ColourBlendState m_cbState;
		renderer::RasterisationState m_rsState;
		renderer::DepthStencilState m_dsState;
		renderer::MultisampleState m_msState;
		renderer::TessellationState m_tsState;
		std::unique_ptr< renderer::Viewport > m_viewport;
		std::unique_ptr< renderer::Scissor > m_scissor;
	};
}

#endif
