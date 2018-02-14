/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlViewportCommand.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	ViewportCommand::ViewportCommand( Device const & device
		, renderer::Viewport const & viewport )
		: m_device{ device }
		, m_viewport{ viewport }
	{
	}

	void ViewportCommand::apply()const
	{
		auto & save = m_device.getCurrentViewport();

		if ( m_viewport != save )
		{
			glLogCommand( "ViewportCommand" );
			glLogCall( gl::Viewport
				, m_viewport.getOffset()[0]
				, m_viewport.getOffset()[1]
				, m_viewport.getSize()[0]
				, m_viewport.getSize()[1] );
			save = m_viewport;
		}
	}

	CommandPtr ViewportCommand::clone()const
	{
		return std::make_unique< ViewportCommand >( *this );
	}
}
