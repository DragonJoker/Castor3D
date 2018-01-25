/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlViewportCommand.hpp"

namespace gl_renderer
{
	ViewportCommand::ViewportCommand( renderer::Viewport const & viewport )
		: m_viewport{ viewport }
	{
	}

	void ViewportCommand::apply()const
	{
		glLogCall( gl::Viewport
			, m_viewport.getOffset()[0]
			, m_viewport.getOffset()[1]
			, m_viewport.getSize()[0]
			, m_viewport.getSize()[1] );
	}

	CommandPtr ViewportCommand::clone()const
	{
		return std::make_unique< ViewportCommand >( *this );
	}
}
