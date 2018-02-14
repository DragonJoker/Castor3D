/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ComputePipeline_HPP___
#define ___Renderer_ComputePipeline_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Un pipeline de rendu.
	*/
	class ComputePipeline
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
		*/
		ComputePipeline( Device const & device
			, PipelineLayout const & layout
			, ShaderProgram const & program );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~ComputePipeline() = default;
	};
}

#endif
