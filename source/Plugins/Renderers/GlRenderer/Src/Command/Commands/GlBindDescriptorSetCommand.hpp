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
	*	Commande d'activation d'un set de descripteurs.
	*/
	class BindDescriptorSetCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] descriptorSet
		*	Le descriptor set.
		*\param[in] layout
		*	Le layout de pipeline.
		*\param[in] bindingPoint
		*	Le point d'attache du set.
		*/
		BindDescriptorSetCommand( renderer::DescriptorSet const & descriptorSet
			, renderer::PipelineLayout const & layout
			, renderer::PipelineBindPoint bindingPoint );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		DescriptorSet const & m_descriptorSet;
		PipelineLayout const & m_layout;
		renderer::PipelineBindPoint m_bindingPoint;
	};
}
