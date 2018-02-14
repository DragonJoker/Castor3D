/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PipelineLayout_HPP___
#define ___Renderer_PipelineLayout_HPP___
#pragma once

#include "ColourBlendState.hpp"
#include "RasterisationState.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	The pipeline layout.
	*\~french
	*\brief
	*	Layout de pipeline.
	*/
	class PipelineLayout
	{
	protected:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The parent LogicalDevice.
		*\param[in] setLayouts
		*	The descriptor sets layouts.
		*\param[in] pushConstantRanges
		*	The push constants ranges.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] setLayouts
		*	Les layouts des descripteurs du pipeline.
		*\param[in] pushConstantRanges
		*	Les intervalles de push constants.
		*/
		PipelineLayout( Device const & device
			, DescriptorSetLayoutCRefArray const & setLayouts
			, PushConstantRangeCRefArray const & pushConstantRanges );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~PipelineLayout() = default;
		/**
		*\~english
		*\brief
		*	Creates a graphics pipeline using this layout.
		*\param[in] program
		*	The shader program.
		*\param[in] vertexBuffers
		*	The vertex buffers used.
		*\param[in] renderPass
		*	The render pass.
		*\param[in] topology
		*	The rendering topology.
		*\param[in] rasterisationState
		*	The rasterisation state.
		*\param[in] colourBlendState
		*	The colour blend state.
		*\return
		*	The created pipeline.
		*\~french
		*\brief
		*	Crée un pipeline graphique utilisant ce layout.
		*\param[in] program
		*	Le programme shader.
		*\param[in] vertexBuffers
		*	Les tampons de sommets utilisés.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*\param[in] rasterisationState
		*	L'état de rastérisation.
		*\param[in] colourBlendState
		*	L'état de mélange de couleurs.
		*\return
		*	Le pipeline créé.
		*/
		virtual PipelinePtr createPipeline( ShaderProgram const & program
			, VertexLayoutCRefArray const & vertexLayouts
			, RenderPass const & renderPass
			, InputAssemblyState const & inputAssemblyState
			, RasterisationState const & rasterisationState = RasterisationState{}
			, ColourBlendState const & colourBlendState = ColourBlendState::createDefault() )const = 0;
		/**
		*\~english
		*\brief
		*	Creates a compute pipeline using this layout.
		*\param[in] program
		*	The shader program.
		*\return
		*	The created pipeline.
		*\~french
		*\brief
		*	Crée un pipeline de calcul utilisant ce layout.
		*\param[in] program
		*	Le programme shader.
		*\return
		*	Le pipeline créé.
		*/
		virtual ComputePipelinePtr createPipeline( ShaderProgram const & program )const = 0;
	};
}

#endif
