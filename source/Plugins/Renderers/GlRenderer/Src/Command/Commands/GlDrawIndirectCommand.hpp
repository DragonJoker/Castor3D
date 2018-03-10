/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	class DrawIndirectCommand
		: public CommandBase
	{
	public:
		DrawIndirectCommand( renderer::BufferBase const & buffer
			, uint32_t offset
			, uint32_t drawCount
			, uint32_t stride
			, renderer::PrimitiveTopology mode );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Buffer const & m_buffer;
		uint32_t m_offset;
		uint32_t m_drawCount;
		uint32_t m_stride;
		GlPrimitiveTopology m_mode;
	};
}
