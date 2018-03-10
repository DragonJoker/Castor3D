/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Pipeline/Scissor.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande d'application d'un scissor.
	*/
	class ScissorCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] scissor
		*	Le scissor.
		*/
		ScissorCommand( Device const & device
			, renderer::Scissor const & scissor );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Device const & m_device;
		renderer::Scissor m_scissor;
	};
}
