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
	class PushConstantsCommand
		: public CommandBase
	{
	public:
		PushConstantsCommand( renderer::PipelineLayout const & layout
			, renderer::PushConstantsBufferBase const & pcb );
		void apply()const override;
		CommandPtr clone()const override;

	private:
		renderer::PushConstantsBufferBase const & m_pcb;
		renderer::ByteArray m_data;
	};
}
