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
	*	Commande de fin d'une requ�te.
	*/
	class EndQueryCommand
		: public CommandBase
	{
	public:
		EndQueryCommand( renderer::QueryPool const & pool
			, uint32_t query );
		void apply()const override;
		CommandPtr clone()const override;

	private:
		GlQueryType m_target;
	};
}
