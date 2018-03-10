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
	*	Classe de base d'une commande.
	*/
	class ImageMemoryBarrierCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		ImageMemoryBarrierCommand( renderer::PipelineStageFlags after
			, renderer::PipelineStageFlags before
			, renderer::ImageMemoryBarrier const & transitionBarrier );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		GlMemoryBarrierFlags m_flags;
	};
}
