/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Pipeline/Viewport.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande d'application d'un viewport.
	*/
	class ViewportCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] viewport
		*	Le viewport.
		*/
		ViewportCommand( Device const & device
			, renderer::Viewport const & viewport );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Device const & m_device;
		renderer::Viewport m_viewport;
	};
}
