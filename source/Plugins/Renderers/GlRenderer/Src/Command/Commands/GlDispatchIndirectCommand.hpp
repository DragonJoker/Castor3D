/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	class DispatchIndirectCommand
		: public CommandBase
	{
	public:
		DispatchIndirectCommand( renderer::BufferBase const & buffer
			, uint32_t offset );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Buffer const & m_buffer;
		uint32_t m_offset;
	};
}
