/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Pipeline_HPP___
#define ___Renderer_Pipeline_HPP___
#pragma once

#include "ColourBlendState.hpp"
#include "RasterisationState.hpp"

namespace renderer
{
	/**
	*\brief
	*	Un pipeline de rendu.
	*/
	class Pipeline
	{
	protected:
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
			, PipelineLayout const & layout
			, ShaderProgram const & program
			, VertexLayoutCRefArray const & vertexLayouts
			, RenderPass const & renderPass
			, PrimitiveTopology topology
			, RasterisationState const & rasterisationState = RasterisationState{}
			, ColourBlendState const & colourBlendState = ColourBlendState::createDefault() );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~Pipeline() = default;
		/**
		*\brief
		*	Crée le pipeline.
		*/
		virtual Pipeline & finish() = 0;
		/**
		*\brief
		*	Définit le MultisampleState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		virtual Pipeline & multisampleState( MultisampleState const & state ) = 0;
		/**
		*\brief
		*	Définit le DepthStencilState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		virtual Pipeline & depthStencilState( DepthStencilState const & state ) = 0;
		/**
		*\brief
		*	Définit le TessellationState.
		*\param[in] state
		*	La nouvelle valeur.
		*/
		virtual Pipeline & tessellationState( TessellationState const & state ) = 0;
		/**
		*\brief
		*	Définit le Viewport.
		*\param[in] viewport
		*	La nouvelle valeur.
		*/
		virtual Pipeline & viewport( Viewport const & viewport ) = 0;
		/**
		*\brief
		*	Définit le Scissor.
		*\param[in] scissor
		*	La nouvelle valeur.
		*/
		virtual Pipeline & scissor( Scissor const & scissor ) = 0;
		/**
		*\return
		*	Le type de primitives dessinées via ce pipeline.
		*/
		inline PrimitiveTopology getPrimitiveType()const
		{
			return m_topology;
		}

	private:
		PrimitiveTopology m_topology;
	};
}

#endif
