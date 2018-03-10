/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Démarre une passe de rendu en bindant son framebuffer, et en le vidant au besoin.
	*/
	class BeginRenderPassCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] frameBuffer
		*	Le tampon d'image affecté par le rendu.
		*\param[in] clearValues
		*	Les valeurs de vidage, une par attache de la passe de rendu.
		*\param[in] contents
		*	Indique la manière dont les commandes de la première sous-passe sont fournies.
		*/
		BeginRenderPassCommand( renderer::RenderPass const & renderPass
			, renderer::FrameBuffer const & frameBuffer
			, renderer::ClearValueArray const & clearValues
			, renderer::SubpassContents contents
			, renderer::SubpassDescription const & subpass );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		RenderPass const & m_renderPass;
		renderer::SubpassDescription const & m_subpass;
		FrameBuffer const & m_frameBuffer;
		renderer::ClearValueArray m_clearValues;
	};
}
