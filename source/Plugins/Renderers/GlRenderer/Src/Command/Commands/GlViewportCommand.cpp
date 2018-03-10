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
				, m_viewport.getOffset().x
				, m_viewport.getOffset().y
				, m_viewport.getSize().width
				, m_viewport.getSize().height );
			glLogCall( gl::DepthRange
				, m_viewport.getDepthBounds()[0]
				, m_viewport.getDepthBounds()[1] );
			save = m_viewport;
		}
	}

	CommandPtr ViewportCommand::clone()const
	{
		return std::make_unique< ViewportCommand >( *this );
	}
}
