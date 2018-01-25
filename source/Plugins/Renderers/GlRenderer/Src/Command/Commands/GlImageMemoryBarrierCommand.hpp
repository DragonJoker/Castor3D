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
		ImageMemoryBarrierCommand();

		void apply()const override;
		CommandPtr clone()const override;
	};
}
