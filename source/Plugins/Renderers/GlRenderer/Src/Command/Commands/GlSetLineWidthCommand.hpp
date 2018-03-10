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
	class SetLineWidthCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] scissor
		*	Le scissor.
		*/
		SetLineWidthCommand( float width );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		float m_width;
	};
}
