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
	*	Commande d'activation d'un pipeline: shaders, tests, états, ...
	*/
	class BindPipelineCommand
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
		BindPipelineCommand( renderer::Pipeline const & pipeline
			, renderer::PipelineBindPoint bindingPoint );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Pipeline const & m_pipeline;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_program;
		renderer::PipelineBindPoint m_bindingPoint;
	};
}
