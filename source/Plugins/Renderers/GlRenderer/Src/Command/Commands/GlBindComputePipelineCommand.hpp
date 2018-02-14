/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Commande d'activation d'un pipeline de calcul.
	*/
	class BindComputePipelineCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] pipeline
		*	Le pipeline à activer.
		*\param[in] bindingPoint
		*	Le point d'attache du pipeline.
		*/
		BindComputePipelineCommand( Device const & device
			, renderer::ComputePipeline const & pipeline
			, renderer::PipelineBindPoint bindingPoint );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Device const & m_device;
		ComputePipeline const & m_pipeline;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_program;
		renderer::PipelineBindPoint m_bindingPoint;
	};
}
