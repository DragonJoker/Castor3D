/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	void apply( Device const & device
		, renderer::ColourBlendState const & state );
	void apply( Device const & device
		, renderer::RasterisationState const & state );
	void apply( Device const & device
		, renderer::MultisampleState const & state );
	void apply( Device const & device
		, renderer::DepthStencilState const & state );
	void apply( Device const & device
		, renderer::TessellationState const & state );
	/**
	*\brief
	*	Commande d'activation d'un pipeline: shaders, tests, �tats, ...
	*/
	class BindPipelineCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] pipeline
		*	Le pipeline � activer.
		*\param[in] bindingPoint
		*	Le point d'attache du pipeline.
		*/
		BindPipelineCommand( Device const & device
			, renderer::Pipeline const & pipeline
			, renderer::PipelineBindPoint bindingPoint );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Device const & m_device;
		Pipeline const & m_pipeline;
		PipelineLayout const & m_layout;
		ShaderProgram const & m_program;
		renderer::PipelineBindPoint m_bindingPoint;
	};
}
