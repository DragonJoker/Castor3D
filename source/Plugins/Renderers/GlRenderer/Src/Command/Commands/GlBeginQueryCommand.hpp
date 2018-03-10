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
	*	Commande de d�marrage d'une requ�te.
	*/
	class BeginQueryCommand
		: public CommandBase
	{
	public:
		BeginQueryCommand( renderer::QueryPool const & pool
			, uint32_t query
			, renderer::QueryControlFlags flags );
		void apply()const override;
		CommandPtr clone()const override;

	private:
		GlQueryType m_target;
		GLuint m_query;
	};
}
