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
	*	Commande de fin d'une passe de rendu, unbind son framebuffer.
	*/
	class ResetQueryPoolCommand
		: public CommandBase
	{
	public:
		ResetQueryPoolCommand( renderer::QueryPool const & pool
			, uint32_t firstQuery
			, uint32_t queryCount );
		void apply()const override;
		CommandPtr clone()const override;
	};
}
