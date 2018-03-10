/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlSetLineWidthCommand.hpp"

namespace gl_renderer
{
	SetLineWidthCommand::SetLineWidthCommand( float width )
		: m_width{ width }
	{
	}

	void SetLineWidthCommand::apply()const
	{
		glLogCommand( "SetLineWidthCommand" );
		glLogCall( gl::LineWidth, m_width );
	}

	CommandPtr SetLineWidthCommand::clone()const
	{
		return std::make_unique< SetLineWidthCommand >( *this );
	}
}
