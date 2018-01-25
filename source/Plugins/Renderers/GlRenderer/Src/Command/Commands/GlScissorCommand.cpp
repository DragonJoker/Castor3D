/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlScissorCommand.hpp"

namespace gl_renderer
{
	ScissorCommand::ScissorCommand( renderer::Scissor const & scissor )
		: m_scissor{ scissor }
	{
	}

	void ScissorCommand::apply()const
	{
		glLogCall( gl::Scissor
			, m_scissor.getOffset()[0]
			, m_scissor.getOffset()[1]
			, m_scissor.getSize()[0]
			, m_scissor.getSize()[1] );
	}

	CommandPtr ScissorCommand::clone()const
	{
		return std::make_unique< ScissorCommand >( *this );
	}
}
