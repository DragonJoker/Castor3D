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
	class BufferMemoryBarrierCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		BufferMemoryBarrierCommand();

		void apply()const override;
		CommandPtr clone()const override;
	};
}
